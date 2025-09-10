/* Copyright 2025 M. MAD */

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

// Pin Definitions
#define JOY_Y A0  // Joystick Y-axis analog input
#define JOY_X A1  // Joystick X-axis analog input
#define JOY_BUTTON 2  // Joystick button (digital input)

#define PM1 3  // Propulsion motor direction 1, L298N IN1 pin
#define PM2 4  // Propulsion motor direction 2, L298N IN2 pin
#define PMPWM 5  // Propulsion motor PWM, L298N ENA pin (PWM)

#define SM1 8  // Steering motor direction 1, L298N IN3 pin
#define SM2 7  // Steering motor direction 2, L298N IN4 pin
#define SMPWM 6  // Steering motor PWM, L298N ENB pin (PWM)

// Configuration
#define PROP_MIN 60  // Minimum propulsion PWM (dead zone threshold)
#define STEER_MIN 15  // Minimum steering PWM (dead zone threshold)
#define STEER_MAX 200  // Maximum steering PWM (prevent overdrive)

int prevX = 512;  // Previous X position (centered)

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

void setup() {
  // Set all motor control pins as outputs
  pinMode(PM1, OUTPUT);
  pinMode(PM2, OUTPUT);
  pinMode(PMPWM, OUTPUT);
  pinMode(SM1, OUTPUT);
  pinMode(SM2, OUTPUT);
  pinMode(SMPWM, OUTPUT);
  
  // Initialize motors in stop state
  propulsionStop();
  steeringStop();
  
  // Initialize serial communication for debugging
  DEBUG_SETUP(9600);
}

void loop() {
  // Read joystick values
  int yVal = analogRead(JOY_Y);  // Propulsion control
  int xVal = analogRead(JOY_X);  // Steering control
  
  // Propulsion Motor Control (Y-axis)
  int propulsionPWM = map(yVal, 0, 1023, 255, -255);  // Invert Y-axis
  
  // Apply deadzone and direction control
  if (abs(propulsionPWM) < PROP_MIN) {
    propulsionStop();
    DEBUG_PRINTLN("PS");
  } else if (propulsionPWM > 0) {
    propulsionForward(abs(propulsionPWM));
    DEBUG_PRINT("F=");
    DEBUG_PRINTLN(abs(propulsionPWM));
  } else {
    propulsionBackward(abs(propulsionPWM));
    DEBUG_PRINT("B=");
    DEBUG_PRINTLN(abs(propulsionPWM));
  }
  
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
  
  // Small delay to stabilize readings
  delay(20);
}
