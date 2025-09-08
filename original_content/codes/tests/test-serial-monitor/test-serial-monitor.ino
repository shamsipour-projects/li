/* Copyright 2025 M. MAD */

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
}

void loop() {
  // Receive and echo messages
  if (Serial.available() > 0) {
    String received = Serial.readStringUntil('\n'); // Read until newline
    received.trim(); // Remove extra whitespace

    Serial.print("Arduino Received: ");
    Serial.println(received);
  }

  // Send periodic messages
  static unsigned long lastSent = 0;
  if (millis() - lastSent > 2000) { // Every 2 seconds
    lastSent = millis();
    Serial.println("Hello from Arduino!");
  }
}
