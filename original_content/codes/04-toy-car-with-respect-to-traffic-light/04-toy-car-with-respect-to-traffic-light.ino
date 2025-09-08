/* Copyright 2025 M. MAD */

// For this experiment, leave the 'ENABLE_DEBUG' as '0', because digital pins
// '12' and '13' (LED_BUILTIN) conflict with Color sensor (TCS3200) 'S2' and
// 'S3' pins
#define ENABLE_DEBUG 0
#if ENABLE_DEBUG                                                                                                                                    
  #define DEBUG_PIN 12
  #define DEBUG_LED LED_BUILTIN
  #define DEBUG_SETUP(baud) do { \
    pinMode(DEBUG_PIN, INPUT_PULLUP); \
    pinMode(DEBUG_LED, OUTPUT); \
    Serial.begin(baud); \
  } while(0)

  #define DEBUG_PRINT(x) do {Serial.print(x); Serial.flush();} while(0)
  #define DEBUG_PRINTLN(x) do {Serial.println(x); Serial.flush();} while(0)
  #define DEBUG_FLUSH() Serial.flush()
  #define DEBUG(x) do { \
    if (!digitalRead(DEBUG_PIN)) { \
      digitalWrite(DEBUG_LED, HIGH); \
      x; \
      Serial.flush(); \
      digitalWrite(DEBUG_LED, LOW); \
    } \
  } while(0)
#else
  #define DEBUG_SETUP(baud)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_FLUSH()
  #define DEBUG(x)
#endif

// ==================================================
// Types (MUST be placed before ANY function defintion, due to Arduino “auto-prototyping”)
// ==================================================
enum class Colors : unsigned char { RED, GREEN, BLUE };
//enum class Colors { RED, GREEN, BLUE };

// ==================================================
// Misc. global values
// ==================================================
#define DEBOUNCE_DELAY 20  // Small delay to stabilize readings

// ==================================================
// Joystick
// ==================================================
#define JOY_Y A0  // Joystick Y-axis analog input
#define JOY_X A1  // Joystick X-axis analog input
#define JOY_BUTTON 2  // Joystick button (digital input)

int yVal = 512;
int xVal = 512;
int prevX = 512;  // Previous X position (centered)

void joySetup() {
  // Analog inputs do not need any setup
  pinMode(JOY_BUTTON, INPUT_PULLUP);
}

void readJoyStick() {
  yVal = analogRead(JOY_Y);  // Propulsion control
  xVal = analogRead(JOY_X);  // Steering control
}

// ==================================================
// Color sensor (TCS3200)
// ==================================================
#define S0 10
#define S1 11
#define S2 12  // Conflicts with DEBUG_PIN
#define S3 13  // Conflicts with LED_BUILTIN used in debugging
#define OUT 9

#define RED_THRESHOLD 50    // Calibrate for your environment
#define GREEN_THRESHOLD 50  // Calibrate for your environment
#define COLOR_CHECK_INTERVAL 200  // Check for traffic light every 200ms

bool isRedDetected = false;
bool prevColorDetection = false;
unsigned long lastColorCheck = 0;

void colorSensorSetup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  
  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

// Read color intensity
int readColor(Colors color) {
  switch(color) {
    case Colors::RED: // Red filter
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      break;
    case Colors::GREEN: // Green filter
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      break;
    case Colors::BLUE: // Blue filter
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      break;
  }
  // Return pulse width (lower value = more color detected)
  return pulseIn(OUT, LOW);
}

// Check for red traffic light
bool checkRedLight() {
  int red = readColor(Colors::RED);
  int green = readColor(Colors::GREEN);
  
  DEBUG_PRINT("R: ");
  DEBUG_PRINT(red);
  DEBUG_PRINT(" G: ");
  DEBUG_PRINTLN(green);
  
  // Red detected when red value is low and significantly lower than green
  return (red < RED_THRESHOLD) && (red < green * 0.7);
}

bool colorCheck() {
  if (millis() - lastColorCheck > COLOR_CHECK_INTERVAL) {
    isRedDetected = checkRedLight();
    
    lastColorCheck = millis();
    if (isRedDetected) {
      propulsionStop();
      steeringStop();
      if (isRedDetected != prevColorDetection) {  // if detection is new
        DEBUG_PRINTLN("RED LIGHT - STOPPED");
        prevColorDetection = isRedDetected;
      }
    }
  }
  return isRedDetected;
}

// ==================================================
// Propulsion motor
// ==================================================
#define PM1 3  // Propulsion motor direction 1, L298N IN1 pin
#define PM2 4  // Propulsion motor direction 2, L298N IN2 pin
#define PMPWM 5  // Propulsion motor PWM, L298N ENA pin (PWM)

#define PROP_MIN 60  // Minimum propulsion PWM (dead zone threshold)

void propulsionMotorSetup() {
  pinMode(PM1, OUTPUT);
  pinMode(PM2, OUTPUT);
  pinMode(PMPWM, OUTPUT);
  propulsionStop();
}

// Propulsion motor control functions
void propulsionStop() {
  digitalWrite(PM1, LOW);
  digitalWrite(PM2, LOW);
  analogWrite(PMPWM, 0);  // Ensure the motor got disabled
}

void propulsionForward (unsigned char pwm = 255) {
  digitalWrite(PM1, HIGH);
  digitalWrite(PM2, LOW);
  analogWrite(PMPWM, pwm);
}

void propulsionBackward(unsigned char pwm = 255) {
  digitalWrite(PM1, LOW);
  digitalWrite(PM2, HIGH);
  analogWrite(PMPWM, pwm);
}

void handlePropulsion() {
  // Propulsion Motor Control (Y-axis)
  int propulsionPWM = map(yVal, 0, 1023, 255, -255);  // Invert Y-axis
  if (abs(propulsionPWM) < PROP_MIN) {  // Apply deadzone
    propulsionStop();
    DEBUG_PRINTLN("PS");
  } else if (propulsionPWM > 0) {
    if (!isRedDetected) {  // Respect the traffic light
      // This condition shall not be merged with the upper 'else if' condition,
      // becasue it can cause the car to go backward when 'isRedDetected' and
      // user send the command to go forward (it will cause the final 'else'
      // clause to be executed, we have to either use this technic or to
      // transform the final 'else' to 'else if (propulsionPWM < 0)', this
      // approach is more clear and generally better)
      propulsionForward(abs(propulsionPWM));
      DEBUG_PRINT("F=");
      DEBUG_PRINTLN(abs(propulsionPWM));
    } else {
      propulsionStop();
    }
  } else {
    propulsionBackward(abs(propulsionPWM));
    DEBUG_PRINT("B=");
    DEBUG_PRINTLN(abs(propulsionPWM));
  }
}

// ==================================================
// Steering motor
// ==================================================
#define SM1 8  // Steering motor direction 1, L298N IN3 pin
#define SM2 7  // Steering motor direction 2, L298N IN4 pin
#define SMPWM 6  // Steering motor PWM, L298N ENB pin (PWM)

#define STEER_MIN 15  // Minimum steering PWM (dead zone threshold)
#define STEER_MAX 200  // Maximum steering PWM (prevent overdrive)

void steeringMotorSetup() {
  pinMode(SM1, OUTPUT);
  pinMode(SM2, OUTPUT);
  pinMode(SMPWM, OUTPUT);
  steeringStop();
}

// Steering motor control functions
void steeringStop() {
  digitalWrite(SM1, LOW);
  digitalWrite(SM2, LOW);
  analogWrite(SMPWM, 0);  // Ensure the motor got disabled
}

void steeringRight (unsigned char pwm = 255) {
  digitalWrite(SM1, HIGH);
  digitalWrite(SM2, LOW);
  analogWrite(SMPWM, pwm);
}

void steeringLeft(unsigned char pwm = 255) {
  digitalWrite(SM1, LOW);
  digitalWrite(SM2, HIGH);
  analogWrite(SMPWM, pwm);
}

void handleSteering() {
  // Steering Motor Control (X-axis derivative)
  int deltaX = xVal - prevX;  // Calculate position change
  prevX = xVal;  // Store current position
  
  int steeringPWM = abs(deltaX) * 0.25;  // Scale derivative to PWM
  steeringPWM = constrain(steeringPWM, 0, STEER_MAX);  // Limit PWM range
  
  // Apply steering deadzone and direction
  if (steeringPWM < STEER_MIN) {
    steeringStop();
    DEBUG_PRINTLN("SS");
  } else if (deltaX > 0) {
    steeringRight(steeringPWM);
    DEBUG_PRINT("R=");
    DEBUG_PRINTLN(steeringPWM);
  } else {
    steeringLeft(steeringPWM);
    DEBUG_PRINT("L=");
    DEBUG_PRINTLN(steeringPWM);
  }
}


// ==================================================
// setup()
// ==================================================
void setup() {
  joySetup();

  propulsionMotorSetup();
  steeringMotorSetup();

  colorSensorSetup();

  DEBUG_SETUP(9600);
}

// ==================================================
// loop()
// ==================================================
void loop() {
  colorCheck();
  readJoyStick();

  handlePropulsion();
  handleSteering();

  // Small delay to stabilize readings
  delay(DEBOUNCE_DELAY);
}
