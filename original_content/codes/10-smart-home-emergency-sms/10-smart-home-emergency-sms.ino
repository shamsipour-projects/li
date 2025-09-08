/* Copyright 2025 M. MAD */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <HX711.h>
#include <BH1750.h>
#include <SoftwareSerial.h>

// Pin definitions
#define TTP223_PIN 2
#define JOYSTICK_Y_PIN A0
#define DHT22_PIN 3
#define MQ8_PIN A1
#define HX711_DT_PIN 4
#define HX711_SCK_PIN 5
#define GP2Y0E03_SDA A4
#define GP2Y0E03_SCL A5
#define SIM800_TX_PIN 6
#define SIM800_RX_PIN 7

// Sensor objects
#define DHTTYPE DHT22
DHT dht(DHT22_PIN, DHTTYPE);
HX711 loadCell;
BH1750 lightMeter;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change address if needed
SoftwareSerial sim800l(SIM800_TX_PIN, SIM800_RX_PIN); // SIM800L Serial

// Variables
int currentScreen = 0;
const int numScreens = 7; // Increased to include distance sensor
int lastYValue = 0;
bool joyMoved = false;
long loadCellValue = 0;
float distance = 0;
bool smsSent = false;
unsigned long lastDistanceCheck = 0;
const unsigned long distanceCheckInterval = 500; // Check distance every 500ms
const float DISTANCE_THRESHOLD = 20.0; // Distance in cm to trigger SMS (adjust as needed)
String phoneNumber = "+1234567890"; // Replace with your phone number

// GP2Y0E03 I2C address
#define GP2Y0E03_ADDRESS 0x40

void setup() {
  Serial.begin(9600);
  sim800l.begin(9600); // SIM800L baud rate
  
  // Initialize sensors
  pinMode(TTP223_PIN, INPUT);
  dht.begin();
  
  // Initialize HX711
  loadCell.begin(HX711_DT_PIN, HX711_SCK_PIN);
  loadCell.set_scale(); // You'll need to calibrate this with your specific load cell
  loadCell.tare();      // Reset the scale to 0
  
  lightMeter.begin();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");
  
  // Initialize SIM800L
  delay(1000);
  sim800l.println("AT");
  delay(500);
  sim800l.println("AT+CMGF=1"); // Set SMS text mode
  delay(500);
  sim800l.println("AT+CNMI=1,2,0,0,0"); // Set SMS notification
  delay(500);
  
  // Initialize GP2Y0E03
  Wire.begin();
  
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

  // Update load cell reading if it's ready
  if (loadCell.is_ready()) {
    loadCellValue = loadCell.get_units(1); // Get 1 reading
  }

  // Check distance at regular intervals
  if (millis() - lastDistanceCheck >= distanceCheckInterval) {
    lastDistanceCheck = millis();
    distance = readGP2Y0E03Distance();
    
    // Check if distance is below threshold and SMS hasn't been sent yet
    if (distance > 0 && distance < DISTANCE_THRESHOLD && !smsSent) {
      sendEmergencySMS(distance);
      smsSent = true;
    } else if (distance >= DISTANCE_THRESHOLD) {
      smsSent = false; // Reset SMS flag when object moves away
    }
  }

  // Display current screen
  switch (currentScreen) {
    case 0: showTTP223(); break;
    case 1: showJoystick(); break;
    case 2: showDHT22(); break;
    case 3: showMQ8(); break;
    case 4: showGY30(); break;
    case 5: showLoadCell(); break;
    case 6: showDistance(); break;
  }
  
  delay(200);
}

// Function to read distance from GP2Y0E03 sensor
float readGP2Y0E03Distance() {
  Wire.beginTransmission(GP2Y0E03_ADDRESS);
  Wire.write(0x5E); // Register for distance measurement
  Wire.endTransmission();
  
  delay(1); // Short delay
  
  Wire.requestFrom(GP2Y0E03_ADDRESS, 2);
  if (Wire.available() >= 2) {
    byte highByte = Wire.read();
    byte lowByte = Wire.read();
    
    // Combine bytes and convert to distance (cm)
    int value = (highByte << 4) | (lowByte & 0x0F);
    return value / 16.0; // Convert to cm
  }
  return -1; // Error reading sensor
}

// Function to send emergency SMS
void sendEmergencySMS(float dist) {
  lcd.clear();
  lcd.print("Sending SMS...");
  
  sim800l.println("AT+CMGF=1"); // Set SMS text mode
  delay(500);
  
  sim800l.print("AT+CMGS=\"");
  sim800l.print(phoneNumber);
  sim800l.println("\"");
  delay(500);
  
  sim800l.print("WARNING: Object detected at ");
  sim800l.print(dist);
  sim800l.println(" cm!");
  sim800l.println("Please check your system.");
  delay(500);
  
  sim800l.write(26); // CTRL+Z to send message
  delay(500);
  
  lcd.clear();
  lcd.print("SMS Sent!");
  delay(2000);
  lcd.clear();
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
  lcd.print(loadCellValue);
  lcd.print(" g"); // Assuming you've calibrated to grams
}

void showDistance() {
  lcd.setCursor(0, 0);
  lcd.print("Distance:");
  lcd.setCursor(0, 1);
  lcd.print(distance);
  lcd.print(" cm");
}