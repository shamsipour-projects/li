/* Copyright 2025 M. MAD */

#include <TM1637Display.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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
// Misc. global values
// ==================================================
#define DEBOUNCE_DELAY 20  // Small delay to stabilize readings

// ==================================================
// TTP223 Touch Lock/Unlock
// ==================================================
#define TOUCH_PIN 3

bool isWinterMode = false;
unsigned long lastTouch = 0;

// CAUTION: TTP223 can be physically configured to be active low or active
// high, or even toggle mode. This file is intended for normal active low, if
// your module is active high, change accordingly and add an external Pulldown
// resistor.
void touchSetup() {
  pinMode(TOUCH_PIN, INPUT_PULLUP);
}

bool touchCheck() {
  if (!digitalRead(TOUCH_PIN) && millis() - lastTouch > DEBOUNCE_DELAY) {
    isWinterMode = !isWinterMode;
    lastTouch = millis();
    DEBUG_PRINT("isWinterMode = ");
    DEBUG_PRINTLN(isWinterMode);
  }
  return isWinterMode;
}

// ==================================================
// Joystick
// ==================================================
#define JOY_Y A0  // Joystick Y-axis analog input
#define JOY_BUTTON 2  // Joystick button (digital input)
#define DEADZONE 50
#define JOY_THRESHOLD 100

int yVal = 512;
int lastJoyY = 512;
int yDiff = 0;
bool isSettingGoalTemp = true;  // true = setting target temp, false = setting sensitivity
unsigned long lastJoyButton = 0;
int targetTemp = 21;  // Default target temperature
int sensitivity = 2;  // Default sensitivity (1-10, representing 0.5-5°C)

void joySetup() {
  // Analog inputs do not need any setup
  pinMode(JOY_BUTTON, INPUT_PULLUP);
}

void joyYCheck() {
  yVal = analogRead(JOY_Y);
  yDiff = yVal - lastJoyY;
  lastJoyY = yVal;
  // Check if joystick has moved beyond the deadzone
  if (abs(yDiff) > JOY_THRESHOLD) {
    if (yVal < (512 - DEADZONE)) {  // Joystick moved up
      if (isSettingGoalTemp) {
        targetTemp = constrain(targetTemp + 1, 1, 99);
      } else {
        sensitivity = constrain(sensitivity + 1, 1, 20);
      }
    } else if (yVal > (512 + DEADZONE)) {  // Joystick moved down
      if (isSettingGoalTemp) {
        targetTemp = constrain(targetTemp - 1, 1, 99);
      } else {
        sensitivity = constrain(sensitivity - 1, 1, 20);
      }
    }
  }
}

bool joyButtonCheck() {
  if (digitalRead(JOY_BUTTON) && millis() - lastJoyButton > DEBOUNCE_DELAY) {
    isSettingGoalTemp = !isSettingGoalTemp;
    lastJoyButton = millis();
    DEBUG_PRINT("isSettingGoalTemp = ");
    DEBUG_PRINTLN(isSettingGoalTemp);
  }
  return isSettingGoalTemp;
}

// ==================================================
// Waterproof temperature sensor (DS18B20)
// ==================================================
#define ONE_WIRE_BUS 4
#define TEMP_READ_INTERVAL 1000

unsigned long lastTempRead = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float currentTemp = 0;

void tempSensorSetup() {
  sensors.begin();
  readTemp();  // Initial temperature reading
}

float readTemp() {
  sensors.requestTemperatures();
  currentTemp = sensors.getTempCByIndex(0);
  return currentTemp;
}

float tempCheck() {
  if (millis() - lastTempRead >= TEMP_READ_INTERVAL) {
    currentTemp = readTemp();
    lastTempRead = millis();
  }
  return currentTemp;
}

// ==================================================
// 4-Digits 7-segment with colon (TM1637)
// ==================================================
#define CLK 5
#define DIO 6
#define BLINK_INTERVAL 750

TM1637Display display(CLK, DIO);

// #define COLOR_CHECK_INTERVAL 200  // Check for traffic light every 200ms

bool blinkState = false;
unsigned long lastBlinkTime = 0;

void displaySetup() {
  display.setBrightness(7);
}

void displayUpdate() {
  // Handle blinking for sensitivity display
  if (!isSettingGoalTemp && (millis() - lastBlinkTime >= BLINK_INTERVAL)) {
    blinkState = !blinkState;
    lastBlinkTime = millis();
  }

  // Create display segments
  unsigned char segments[4]; // uint8_t

  // Left side (current temperature)
  int leftNum = constrain(round(currentTemp), 0, 99);
  segments[0] = display.encodeDigit(leftNum / 10);
  segments[1] = display.encodeDigit(leftNum % 10);

  // Right side (target or sensitivity)
  int displayTarget = isSettingGoalTemp ? targetTemp : sensitivity;
  bool showRight = isSettingGoalTemp || blinkState;
  if (showRight) {
    segments[2] = display.encodeDigit(displayTarget / 10);
    segments[3] = display.encodeDigit(displayTarget % 10);
  } else {
    segments[2] = 0;
    segments[3] = 0;
  }

  // Set colon for winter mode
  if (isWinterMode) {
    segments[1] |= 0x80; // Add colon segment
  }

  display.setSegments(segments);
}

// ==================================================
// Relay control
// ==================================================
#define RELAY_PIN 7

float tempDeadzone = 0;
bool relayState = false;

// CAUTION: This file assumes that the relay is connected as an active high
// device (the normally open (NO) contacts are used). Adjust accordingly if you
// have different hardware configuration.

void relaySetup() {
  pinMode(RELAY_PIN, OUTPUT);
}

void relayControl(bool state) {
  digitalWrite(RELAY_PIN, state);
  relayState = state;
  DEBUG_PRINT("relayState = ");
  DEBUG_PRINTLN(relayState);
}

void relayHandle() {
  tempDeadzone = sensitivity * 0.5; // Convert sensitivity to temperature tempDeadzone
  // Winter mode: activate when temperature falls below target
  // Summer mode: activate when temperature rises above target
  relayControl(
    (isWinterMode && currentTemp <= (targetTemp - tempDeadzone))
    || (!isWinterMode && currentTemp >= (targetTemp + tempDeadzone))
  );
}


// ==================================================
// setup()
// ==================================================
void setup() {
  touchSetup();
  joySetup();

  tempSensorSetup();
  
  displaySetup();
  relaySetup();

  DEBUG_SETUP(9600);
}

// ==================================================
// loop()
// ==================================================
void loop() {
  touchCheck();
  joyYCheck();
  joyButtonCheck();

  tempCheck();

  displayUpdate();
  relayHandle();

  // Small delay to stabilize readings
  delay(DEBOUNCE_DELAY);
}
