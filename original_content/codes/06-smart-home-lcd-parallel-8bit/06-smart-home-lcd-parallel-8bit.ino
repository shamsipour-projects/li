/* Copyright 2025 M. MAD */

#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <HX711.h>
#include <BH1750.h>

// Pin definitions
#define TTP223_PIN 2
#define JOYSTICK_Y_PIN A0
#define DHT22_PIN 3
#define MQ8_PIN A1
#define HX711_DT_PIN 4
#define HX711_SCK_PIN 5
#define LCD_RS 6
#define LCD_E 7
#define LCD_D0 8
#define LCD_D1 9
#define LCD_D2 10
#define LCD_D3 11
// Digital pins '12' and '13' are reserved for serial debugging. Using LCD in
// 'parallel 8-bit' mode will require an Arduino Mega because Arduino Uno will
// run out of digital pins. Cosider using LCD in 4-bit mode if you want to /
// have to use an Arduino Uno.
#define LCD_D4 14
#define LCD_D5 15
#define LCD_D6 16
#define LCD_D7 17
#define LCD_RW 18

// Sensor objects
DHT dht(DHT22_PIN, DHT22);
HX711 scale;
BH1750 lightMeter;
// Setting up LCD in 'parallel 8-bit' mode
LiquidCrystal lcd(
  LCD_RS,
  LCD_RW,
  LCD_E,
  LCD_D0,
  LCD_D1,
  LCD_D2,
  LCD_D3,
  LCD_D4,
  LCD_D5,
  LCD_D6,
  LCD_D7
);

// Variables
int currentScreen = 0;
const int numScreens = 6;
int lastYValue = 0;
bool joyMoved = false;

void setup() {
  Serial.begin(9600);
  
  // Initialize sensors
  pinMode(TTP223_PIN, INPUT);
  dht.begin();
  scale.begin(HX711_DT_PIN, HX711_SCK_PIN);
  lightMeter.begin();
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  // Adjust backlight physically using an external power supply and a
  // potentiometer
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read joystick for navigation
  int yValue = analogRead(JOYSTICK_Y_PIN);
  
  // Handle navigation
  if (yValue < 400 && !joyMoved) {
    currentScreen = (currentScreen + 1) % numScreens;
    joyMoved = true;
    lcd.clear();
  } else if (yValue > 600 && !joyMoved) {
    currentScreen = (currentScreen - 1 + numScreens) % numScreens;
    joyMoved = true;
    lcd.clear();
  }
  
  if (yValue >= 400 && yValue <= 600) {
    joyMoved = false;
  }

  // Display current screen
  switch (currentScreen) {
    case 0: showTTP223(); break;
    case 1: showJoystick(); break;
    case 2: showDHT22(); break;
    case 3: showMQ8(); break;
    case 4: showGY30(); break;
    case 5: showLoadCell(); break;
  }
  
  delay(200);
}

void showTTP223() {
  lcd.setCursor(0, 0);
  lcd.print("Touch Sensor: ");
  lcd.setCursor(0, 1);
  lcd.print(digitalRead(TTP223_PIN) ? "ACTIVE " : "INACTIVE");
}

void showJoystick() {
  lcd.setCursor(0, 0);
  lcd.print("Joystick Y:");
  lcd.setCursor(0, 1);
  lcd.print("Value: ");
  lcd.print(analogRead(JOYSTICK_Y_PIN));
}

void showDHT22() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print("C");
  
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(h);
  lcd.print("%");
}

void showMQ8() {
  int sensorValue = analogRead(MQ8_PIN);
  lcd.setCursor(0, 0);
  lcd.print("MQ8 H2 Sensor:");
  lcd.setCursor(0, 1);
  lcd.print("Value: ");
  lcd.print(sensorValue);
}

void showGY30() {
  float lux = lightMeter.readLightLevel();
  lcd.setCursor(0, 0);
  lcd.print("Illuminance:");
  lcd.setCursor(0, 1);
  lcd.print(lux);
  lcd.print(" lx");
}

void showLoadCell() {
  lcd.setCursor(0, 0);
  lcd.print("Load Cell:");
  lcd.setCursor(0, 1);

  if (scale.is_ready()) {
    lcd.print(scale.read());
  } else {
    lcd.print("Not ready");
  }
}