/* Copyright 2025 M. MAD */

#include <FastLED.h>

// --- Configuration for LED Matrix ---
#define NUM_LEDS 16  // 4x4 LED matrix
#define DATA_PIN 6  // Pin connected to the LED data line
#define LED_TYPE WS2812B  // Change if you use a different LED type
#define COLOR_ORDER GRB  // Most WS2812B strips use GRB order

CRGB leds[NUM_LEDS];

// --- Button Pin Definitions ---
#define BUTTON_GREEN_PIN 2  // Button for green (all leds green)
#define BUTTON_YELLOW_PIN 3  // Button for yellow (all leds yellow)
#define BUTTON_RED_PIN 4  // Button for red (all leds red)
#define BUTTON_OFF_PIN 5  // Button for flashing mode

// --- Timing for flashing mode ---
unsigned long previousMillis = 0;
const unsigned long flashInterval = 500; // 500ms flashing interval

// --- Modes ---
// Singleton struct to store corresponding CRGB color for each operation mode
struct ModesStruct {
  CRGB GREEN = CRGB::Green;
  CRGB YELLOW = CRGB::Yellow;
  CRGB RED = CRGB::Red;
  CRGB OFF = CRGB::Black;
};

ModesStruct modes;  // Singleton instantiation

CRGB currentMode = modes.OFF;

bool flashing = false; // true if the current mode is flashing
bool flashState = false; // internal state to toggle between on and off for flash

void setup() {
  // Initialize serial monitor for debugging.
  //Serial.begin(9600);
  
  // Initialize LED library
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  
  // Configure button pins as inputs with internal pull-up resistors.
  pinMode(BUTTON_GREEN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RED_PIN, INPUT_PULLUP);
  pinMode(BUTTON_OFF_PIN, INPUT_PULLUP);
  
  // Start with off mode
  setAllLeds(CRGB::Black);
}

void loop() {
  // Check buttons (active low; adjust if your capacitive setup acts differently)
  if (digitalRead(BUTTON_GREEN_PIN) == LOW) {
    handleModeButtonPress(modes.GREEN);
    delay(200); // Debounce delay
  }
  else if (digitalRead(BUTTON_YELLOW_PIN) == LOW) {
    handleModeButtonPress(modes.YELLOW);
    delay(200);
  }
  else if (digitalRead(BUTTON_RED_PIN) == LOW) {
    handleModeButtonPress(modes.RED);
    delay(200);
  }
  else if (digitalRead(BUTTON_OFF_PIN) == LOW) {
    handleModeButtonPress(modes.OFF);
    delay(200);
  }

  // Update flashing if needed.
  if (flashing && currentMode != modes.OFF) {
    updateFlashing();
  }
  else {
    FastLED.show();
  }
}

// ---------------------------------------------------------------
// Handle a press for one of the colored mode buttons.
void handleModeButtonPress(CRGB pressedMode) {
  if (currentMode == pressedMode) {
    // The same mode button is pressed.
    if (!flashing) {
      // If not already flashing, start flashing.
      flashing = true;
      flashState = true; // Start with the color on
      previousMillis = millis();
    } else {
      // If it's already flashing, stop flashing and revert to solid.
      flashing = false;
      setAllLeds(pressedMode);
    }
  } else {
    // A different color button is pressed: update mode to that color, solid.
    currentMode = pressedMode;
    flashing = false;
    setAllLeds(pressedMode);
  }
}

// ---------------------------------------------------------------
// updateFlashing: toggle between the mode color and off.
void updateFlashing() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= flashInterval) {
    previousMillis = currentMillis;
    flashState = !flashState;
    if (flashState) {
      setAllLeds(currentMode);
    } else {
      setAllLeds(CRGB::Black);
    }
  }
}
// ---------------------------------------------------------------
// setAllLeds: Sets all LEDs in the matrix to the specified color.
void setAllLeds(const CRGB &color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}
