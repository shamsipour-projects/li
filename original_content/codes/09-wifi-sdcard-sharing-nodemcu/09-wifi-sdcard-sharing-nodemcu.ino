/* Copyright 2025 M. MAD */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
String fileContent = "";

// WiFi credentials
const char* ssid = "FileServerAP";
const char* password = "12345678";

void setup() {
  Serial.begin(115200); // Communication with Arduino Mega
  
  // Create WiFi Access Point
  WiFi.softAP(ssid, password);
  
  Serial.println("");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Set up web server endpoints
  server.on("/", handleRoot);
  server.on("/download", handleDownload);
  
  server.begin();
  Serial.println("HTTP server started");
  
  // Request file from Arduino
  requestFileFromArduino();
}

void loop() {
  server.handleClient();
}

void requestFileFromArduino() {
  Serial.println("REQUEST_FILE"); // Send request to Arduino
}

void handleRoot() {
  String html = "<html><head><title>File Server</title></head>";
  html += "<body><h1>File Server</h1>";
  html += "<p>Click <a href='/download'>here</a> to download the file.</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleDownload() {
  server.send(200, "text/plain", fileContent);
}

void serialEvent() {
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    
    if (line == "FILE_START") {
      fileContent = "";
    } else if (line == "FILE_END") {
      // File transmission complete
    } else if (line == "ERROR:SD_FAIL") {
      fileContent = "Error: SD card initialization failed";
    } else if (line == "ERROR:FILE_NOT_FOUND") {
      fileContent = "Error: File not found on SD card";
    } else {
      fileContent += line + "\n";
    }
  }
}
