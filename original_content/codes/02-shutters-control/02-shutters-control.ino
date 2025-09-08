/* Copyright 2025 M. MAD */

#define DEBUG 0
#if DEBUG
  // The `DEBUG_PIN` is active low due to the internal pullup
  #define DEBUG_PIN 12
#endif

// Pin Definitions
#define JOYSTICK_Y A0  // Joystick Y-axis analog input
#define JOYSTICK_BUTTON 2  // Joystick button (digital input)
#define M1 3  // L298N IN1 pin
#define M2 4  // L298N IN2 pin

#define ENABLE_JUMPER 1  // Set to `0` if L298N ENABLE jumper is not installed
#if !ENABLE_JUMPER
  #define PWM1 5  // L298N ENA pin (PWM)
#endif

// Configuration
#define FULL_TRAVEL_TIME 10000 // Time for full open/close (10s)
#define UP_THRESHOLD 400  // Joystick up threshold
#define DOWN_THRESHOLD 600  // Joystick down threshold

// Motor Control Functions
void stopMotor() {
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  #ifdef PWM1
    analogWrite(PWM1, 0);  // Ensure the motor got disabled
  #endif
}

void moveUp() {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  #ifdef PWM1
    analogWrite(PWM1, 255);  // Full speed
  #endif
}

void moveDown() {
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  #ifdef PWM1
    analogWrite(PWM1, 255);  // Full speed
  #endif
}

// System State
enum State { IDLE, MOVING_UP, MOVING_DOWN };
State currentState = IDLE;

// Position Tracking
unsigned long currentPosition = 0;  // Current shutter position (0=closed, FULL_TRAVEL_TIME=open)
unsigned long movementStartTime = 0;  // Movement start timestamp
unsigned long initialPosition = 0;  // Position at movement start
unsigned long targetTime = 0;  // Scheduled stop time

// Input Tracking
bool lastUp = false;
bool lastDown = false;
bool lastButtonState = HIGH;

void setup() {
  // Initialize pins
  pinMode(JOYSTICK_BUTTON, INPUT_PULLUP);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);

  #ifdef PWM1
    pinMode(PWM1, OUTPUT);
  #endif
  
  // Start with motor stopped
  stopMotor();
  
  // Initialize serial for monitoring
  #if DEBUG
    // The `DEBUG_PIN` is active low due to the internal pullup
    pinMode(DEBUG_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
  #endif
}

void loop() {
  // Read current inputs
  bool buttonPressed = (digitalRead(JOYSTICK_BUTTON) == LOW);
  int yValue = analogRead(JOYSTICK_Y);
  bool upPressed = (yValue < UP_THRESHOLD);
  bool downPressed = (yValue > DOWN_THRESHOLD);

  // Handle joystick up edge
  if (upPressed && !lastUp) {
    if (currentState == IDLE && currentPosition < FULL_TRAVEL_TIME) {
      initialPosition = currentPosition;
      movementStartTime = millis();
      targetTime = movementStartTime + (FULL_TRAVEL_TIME - initialPosition);
      currentState = MOVING_UP;
      moveUp();

      #if DEBUG
        if (!digitalRead(DEBUG_PIN)) {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("Starting UP movement");
          Serial.flush();
          digitalWrite(LED_BUILTIN, LOW);
        }
      #endif
    }
  }

  // Handle joystick down edge
  if (downPressed && !lastDown) {
    if (currentState == IDLE && currentPosition > 0) {
      initialPosition = currentPosition;
      movementStartTime = millis();
      targetTime = movementStartTime + initialPosition;
      currentState = MOVING_DOWN;
      moveDown();
      #if DEBUG
        if (!digitalRead(DEBUG_PIN)) {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("Starting DOWN movement");
          Serial.flush();
          digitalWrite(LED_BUILTIN, LOW);
        }
      #endif
    }
  }

  // Handle button press (with edge detection)
  if (buttonPressed && !lastButtonState) {
    if (currentState != IDLE) {
      // Calculate elapsed movement time
      unsigned long elapsed = millis() - movementStartTime;
      
      // Update position based on movement direction
      if (currentState == MOVING_UP) {
        currentPosition = min(initialPosition + elapsed, FULL_TRAVEL_TIME);
        #if DEBUG
          if (!digitalRead(DEBUG_PIN)) {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.print("Stopped UP. New position: ");
            Serial.flush();
            digitalWrite(LED_BUILTIN, LOW);
          }
        #endif
      } else {
        currentPosition = (elapsed < initialPosition) ? 
                          (initialPosition - elapsed) : 0;
        #if DEBUG
          if (!digitalRead(DEBUG_PIN)) {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.print("Stopped DOWN. New position: ");
            Serial.flush();
            digitalWrite(LED_BUILTIN, LOW);
          }
        #endif
      }
      
      #if DEBUG
        if (!digitalRead(DEBUG_PIN)) {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println(currentPosition);
          Serial.flush();
          digitalWrite(LED_BUILTIN, LOW);
        }
      #endif
      stopMotor();
      currentState = IDLE;
    }
  }

  // Check for movement completion
  if (currentState != IDLE && millis() >= targetTime) {
    if (currentState == MOVING_UP) {
      currentPosition = FULL_TRAVEL_TIME;
      #if DEBUG
        if (!digitalRead(DEBUG_PIN)) {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("Reached FULL OPEN");
          Serial.flush();
          digitalWrite(LED_BUILTIN, LOW);
        }
      #endif
    } else {
      currentPosition = 0;
      #if DEBUG
        if (!digitalRead(DEBUG_PIN)) {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("Reached FULL CLOSE");
          Serial.flush();
          digitalWrite(LED_BUILTIN, LOW);
        }
      #endif
    }
    stopMotor();
    currentState = IDLE;
  }

  // Update input states for next iteration
  lastUp = upPressed;
  lastDown = downPressed;
  lastButtonState = buttonPressed;

  // Small delay to stabilize input readings
  delay(10);
}
