/* Copyright 2025 M. MAD */

#include <SPI.h>
#include <SD.h>

const int chipSelect = 53;

void setup() {
  Serial1.begin(115200); // Communication with NodeMCU
  while (!Serial1); // Wait for serial connection
  
  Serial.begin(9600); // For debugging
  Serial.println("Initializing SD card...");
  
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    Serial1.println("ERROR:SD_FAIL");
    return;
  }
  Serial.println("SD card initialized.");
  Serial1.println("READY");
}

void loop() {
  // Check if NodeMCU is requesting a file
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    command.trim();
    
    if (command == "REQUEST_FILE") {
      sendFileToNodeMCU("data.txt"); // Change to your filename
    }
  }
}

void sendFileToNodeMCU(String filename) {
  File file = SD.open(filename);
  if (!file) {
    Serial1.println("ERROR:FILE_NOT_FOUND");
    Serial.println("Error opening file: " + filename);
    return;
  }
  
  Serial1.println("FILE_START");
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Serial1.println(line);
  }
  Serial1.println("FILE_END");
  
  file.close();
  Serial.println("File sent to NodeMCU");
}
