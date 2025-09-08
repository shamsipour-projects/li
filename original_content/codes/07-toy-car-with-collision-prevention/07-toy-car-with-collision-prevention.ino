/* Copyright 2025 M. MAD */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LedControl.h>
#include <VL53L0X.h>  // For GY-530

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
// Obstacle thresholds (cm)
#define FRONT_STOP_DIST 20
#define REAR_STOP_DIST 30

// ==================================================
// TTP223 Touch Lock/Unlock
// ==================================================
#define LOCK_BUTTON 9

bool isCarLocked = true;
unsigned long lastLockPress = 0;

void lockSetup() {
  pinMode(LOCK_BUTTON, INPUT);
}

bool lockCheck() {
  if (digitalRead(LOCK_BUTTON) && millis() - lastLockPress > DEBOUNCE_DELAY) {
    isCarLocked = !isCarLocked;
    lastLockPress = millis();
    updateOLED();
    DEBUG_PRINT("isCarLocked = ");
    DEBUG_PRINTLN(isCarLocked);

    if (isCarLocked) {
      propulsionStop();
      steeringStop();
    }
  }
  return isCarLocked;
}

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
  updateMatrixDisplay(xVal, yVal);// Update the feedback display
}

// ==================================================
// Color sensor (TCS3200)
// ==================================================
// Digital pins 10~13 are SS, MOSI, MISO and SCLK pins on Arduino Uno. Digital
// pins 50~53 are MISO, MODI, SCLK and SS pins on Arduino Mega. These pins are
// used for SPI-communication (with the 'SSD1331' OLED display module, etc)
#define S0 14
#define S1 15
#define S2 16
#define S3 17
#define OUT 18

#define RED_THRESHOLD 50    // Calibrate for your environment
#define GREEN_THRESHOLD 50  // Calibrate for your environment

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
  if (millis() - lastColorCheck > DEBOUNCE_DELAY) {
    isRedDetected = checkRedLight();
    lastColorCheck = millis();
    if (isRedDetected && isRedDetected != prevColorDetection) {
      updateOLED();
      DEBUG_PRINTLN("RED LIGHT - STOPPED");
      prevColorDetection = isRedDetected;
    }
  }
  return isRedDetected;
}

// ==================================================
// Front Distance sensor (HC-SR04)
// ==================================================
#define FRONT_TRIG 19
// Digital pins 20 and 21 are SDA and SCL for I2C communication on Arduino
// Mega.
#define FRONT_ECHO 22
#define REAR_TRIG 23

float frontDist = 0;
bool isFrontObjDetected = false;
bool prevFrontObjDetected = false;
unsigned long lastFrontCheck = 0;

void frontDistanceSetup() {
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
}

float getFrontDistance() {
  digitalWrite(FRONT_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(FRONT_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_TRIG, LOW);
  
  long duration = pulseIn(FRONT_ECHO, HIGH);
  return duration * 0.034 / 2; // cm
}

bool frontCheck() {
  if (millis() - lastFrontCheck > DEBOUNCE_DELAY) {
    frontDist = getFrontDistance();
    isFrontObjDetected = frontDist > 0 && frontDist < FRONT_STOP_DIST;
    lastFrontCheck = millis();
    if (isFrontObjDetected && isFrontObjDetected != prevFrontObjDetected) {
      updateOLED();
      DEBUG_PRINTLN("FRONT OBJ DETECTION - STOPPED");
      prevFrontObjDetected = isFrontObjDetected;
    }
  }
  return isFrontObjDetected;
}

// ==================================================
// Rear Distance sensor (GY530)
// ==================================================
// Connect the sensor pins to Arduino I2C pins
// For Arduino Uno:
//   + VIN -> 5V
//   + GND -> GND
//   + SCL -> A5
//   + SDA -> A4
// For Arduino Mega:
//   + VIN -> 5V
//   + GND -> GND
//   + SCL -> 21 (Digital)
//   + SDA -> 20 (Digital)

VL53L0X rearSensor;

float rearDist = 0;
bool isRearObjDetected = false;
bool prevRearObjDetected = false;
unsigned long lastRearCheck = 0;

void rearDistanceSetup() {
  rearSensor.setTimeout(500);
  if (!rearSensor.init()) {
    DEBUG_PRINTLN("Failed to detect and initialize the rearSensor (GY530)!");
  }
}
 
float getRearDistance() {
  return rearSensor.readRangeSingleMillimeters() / 10.0; // mm to cm
}

bool rearCheck() {
  if (millis() - lastRearCheck > DEBOUNCE_DELAY) {
    rearDist = getRearDistance();
    isRearObjDetected = rearDist > 0 && rearDist < REAR_STOP_DIST;
    lastRearCheck = millis();
    if (isRearObjDetected && isRearObjDetected != prevRearObjDetected) {
      updateOLED();
      DEBUG_PRINTLN("OBJ DETECTION IN REAR - STOPPED");
      prevRearObjDetected = isRearObjDetected;
    }
  }
  return isRearObjDetected;
}

// ==================================================
// MAX7219 8x8 Dot-Matrix LED Display
// ==================================================
#define MATRIX_DIN 24
#define MATRIX_CS 25
#define MATRIX_CLK 26

LedControl lc = LedControl(MATRIX_DIN, MATRIX_CLK, MATRIX_CS, 1);

void matrixDisplaySetup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

void updateMatrixDisplay(int x, int y) {
  lc.clearDisplay(0);
  // Map joystick to 8x8 grid
  int ledX = map(x, 0, 1023, 0, 7);
  int ledY = map(y, 0, 1023, 7, 0); // Invert Y axis
  lc.setLed(0, ledY, ledX, true);
}

// ==================================================
// EMOJI BITMAP DEFINITIONS (16x16)
// ==================================================
// Locked emoji (🔒)
// static const unsigned char emojiLocked[] PROGMEM = {
//   0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 
//   0x0F, 0xE0, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x0F, 0xF0
// };

// Unlocked emoji (🔓)
// static const unsigned char emojiUnlocked[] PROGMEM = {
//   0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 
//   0x0F, 0xE0, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x08, 0x10, 0x0F, 0xF0
// };

// Warning emoji (⚠️)
// static const unsigned char emojiWarning[] PROGMEM = {
//   0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x03, 0xC0, 0x03, 0xC0, 0x06, 0x60, 0x06, 0x60, 0x0C, 0x30, 
//   0x0C, 0x30, 0x18, 0x18, 0x18, 0x18, 0x3F, 0xFC, 0x30, 0x0C, 0x60, 0x06, 0x60, 0x06, 0xC0, 0x03
// };

// Stop emoji (🛑)
// static const unsigned char emojiStop[] PROGMEM = {
//   0x00, 0x00, 0x7F, 0xFE, 0x40, 0x02, 0x5F, 0xFA, 0x50, 0x0A, 0x50, 0x0A, 0x50, 0x0A, 0x50, 0x0A, 
//   0x50, 0x0A, 0x50, 0x0A, 0x50, 0x0A, 0x50, 0x0A, 0x5F, 0xFA, 0x40, 0x02, 0x7F, 0xFE, 0x00, 0x00
// };

// Car emoji (🚗)
// static const unsigned char emojiCar[] PROGMEM = {
//   0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x10, 0x08, 0x2F, 0xD4, 0x30, 0x0C, 0x5F, 0xFA, 0x40, 0x02, 
//   0x40, 0x02, 0x5F, 0xFA, 0x50, 0x0A, 0x50, 0x0A, 0x28, 0x14, 0x10, 0x08, 0x0F, 0xF0, 0x00, 0x00
// };

// Touch emoji (👆)
// static const unsigned char emojiTouch[] PROGMEM = {
//   0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x03, 0xC0, 0x03, 0xC0, 0x07, 0xE0, 0x07, 0xE0, 0x0F, 0xF0, 
//   0x0F, 0xF0, 0x1F, 0xF8, 0x1F, 0xF8, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00
// };

// --------------------------------------------------

// Icon bitmaps (16x16 pixels)
const unsigned char lockIcon[] PROGMEM = {
  0x00, 0x00, 0x03, 0xC0, 0x0C, 0x30, 0x10, 0x08, 0x20, 0x04, 0x20, 0x04, 0x40, 0x02, 0x40, 0x02,
  0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x4F, 0xE2, 0x50, 0x12, 0x50, 0x12, 0x4F, 0xE2
};

const unsigned char unlockIcon[] PROGMEM = {
  0x00, 0x00, 0x03, 0xC0, 0x0C, 0x30, 0x10, 0x08, 0x20, 0x04, 0x20, 0x04, 0x40, 0x02, 0x40, 0x02,
  0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x3F, 0xFC, 0x00, 0x00
};

const unsigned char frontSensorIcon[] PROGMEM = {
  0x00, 0x00, 0x1F, 0xF8, 0x20, 0x04, 0x40, 0x02, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
  0x80, 0x01, 0x80, 0x01, 0x40, 0x02, 0x20, 0x04, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char rearSensorIcon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x20, 0x04, 0x40, 0x02, 0x80, 0x01, 0x80, 0x01,
  0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x40, 0x02, 0x20, 0x04, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00
};

const unsigned char trafficRedIcon[] PROGMEM = {
  0x00, 0x00, 0x03, 0xC0, 0x0F, 0xF0, 0x1F, 0xF8, 0x3F, 0xFC, 0x7F, 0xFE, 0x7F, 0xFE, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFE, 0x7F, 0xFE, 0x3F, 0xFC, 0x1F, 0xF8, 0x0F, 0xF0, 0x03, 0xC0
};

const unsigned char trafficGreenIcon[] PROGMEM = {
  0x03, 0xC0, 0x0F, 0xF0, 0x1F, 0xF8, 0x3F, 0xFC, 0x7F, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFE, 0x3F, 0xFC, 0x1F, 0xF8, 0x0F, 0xF0, 0x03, 0xC0
};

// ==================================================
// SSD1331 Dot-Matrix OLED Display
// ==================================================
// Connect the sensor pins to Arduino SPI pins
// For Arduino Uno:
//   + VIN -> 5V
//   + GND -> GND
//   + SCLK (SCK) -> 13 (Digital)
//   + MOSI -> 11 (Digital)
//   + MISO -> 12 (Digital)
//   + SS -> 10 (Digital)
// For Arduino Mega:
//   + VIN -> 5V
//   + GND -> GND
//   + SCLK (SCK) -> 52 (Digital)
//   + MOSI -> 51 (Digital)
//   + MISO -> 50 (Digital)
//   + SS -> 53 (Digital)
//
// SPI pins DC and RST can be assigned to any free digital pin.

#define OLED_RESET -1
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

void OLEDSetup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(1); // Halt if display fails
  }
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void updateOLED() {
  display.clearDisplay();
  
  // Lock status icon (top-left)
  display.drawBitmap(0, 0, isCarLocked ? lockIcon : unlockIcon, 16, 16, SSD1306_WHITE);
  
  // Front sensor data
  float frontDist = getFrontDistance();
  display.drawBitmap(0, 20, frontSensorIcon, 16, 16, SSD1306_WHITE);
  display.setCursor(20, 25);
  display.print("F: ");
  display.print(frontDist);
  display.print("cm");
  
  // Rear sensor data
  float rearDist = getRearDistance();
  display.drawBitmap(0, 40, rearSensorIcon, 16, 16, SSD1306_WHITE);
  display.setCursor(20, 45);
  display.print("R: ");
  display.print(rearDist);
  display.print("cm");
  
  // Traffic light status
  bool redLight = checkRedLight();
  display.drawBitmap(90, 20, redLight ? trafficRedIcon : trafficGreenIcon, 16, 16, SSD1306_WHITE);
  display.setCursor(110, 25);
  display.print(redLight ? "STOP" : "GO");
  
  // Direction indicator
  int joyY = analogRead(JOY_Y);
  if(!isCarLocked) {
    display.setCursor(90, 45);
    if(joyY < 400) {
      display.print("REVERSE");
    } else if(joyY > 600) {
      display.print("FORWARD");
    } else {
      display.print("STOPPED");
    }
  } else {
    display.setCursor(90, 45);
    display.print("LOCKED");
  }
  
  // Status bar
  display.drawLine(0, 18, 128, 18, SSD1306_WHITE);
  display.display();
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
  analogWrite(PMPWM, constrain(pwm, 0, 255));
}

void propulsionBackward(unsigned char pwm = 255) {
  digitalWrite(PM1, LOW);
  digitalWrite(PM2, HIGH);
  analogWrite(PMPWM, constrain(pwm, 0, 255));
}

void handlePropulsion() {
  // Propulsion Motor Control (Y-axis)
  int propulsionPWM = map(yVal, 0, 1023, 255, -255);  // Invert Y-axis
  if (abs(propulsionPWM) < PROP_MIN) {  // Apply deadzone
    propulsionStop();
    DEBUG_PRINTLN("PS");
  } else if (propulsionPWM > 0) {
    if (!isRedDetected && !isFrontObjDetected) {
      propulsionForward(abs(propulsionPWM));
      DEBUG_PRINT("F=");
      DEBUG_PRINTLN(abs(propulsionPWM));
    } else {
      propulsionStop();
    }
  } else {
    if (!isRearObjDetected) {
      propulsionBackward(abs(propulsionPWM));
      DEBUG_PRINT("B=");
      DEBUG_PRINTLN(abs(propulsionPWM));
    } else {
      propulsionStop();
    }
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
  analogWrite(SMPWM, constrain(pwm, 0, 255));
}

void steeringLeft(unsigned char pwm = 255) {
  digitalWrite(SM1, LOW);
  digitalWrite(SM2, HIGH);
  analogWrite(SMPWM, constrain(pwm, 0, 255));
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
  lockSetup();
  joySetup();

  propulsionMotorSetup();
  steeringMotorSetup();

  colorSensorSetup();
  frontDistanceSetup();
  Wire.begin();
  rearDistanceSetup();

  matrixDisplaySetup();
  OLEDSetup();

  DEBUG_SETUP(9600);
}

// ==================================================
// loop()
// ==================================================
void loop() {
  readJoyStick();

  if (lockCheck()) return;

  colorCheck();
  frontCheck();
  rearCheck();

  handlePropulsion();
  handleSteering();

  // Small delay to stabilize readings
  delay(DEBOUNCE_DELAY);
}
