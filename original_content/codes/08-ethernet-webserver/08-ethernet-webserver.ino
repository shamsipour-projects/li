/* Copyright 2025 M. MAD */

#include <UIPEthernet.h> // Use UIPEthernet library for ENC28J60

// MAC address for your Ethernet module (check sticker or use a random one)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// IP address for the server (must be within your network's range)
IPAddress ip(192, 168, 1, 177); // Change to match your network

// Initialize the Ethernet server on port 80
EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  
  // Start Ethernet with MAC and try to get IP from DHCP, falling back to the
  // defined static IP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed, using static IP");
    Ethernet.begin(mac, ip);
  }
  
  // Start the server
  server.begin();
  
  Serial.print("Server IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Listen for incoming clients
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("New client connected");
    // HTTP request ends with a blank line
    bool currentLineIsBlank = true;
    unsigned long start = millis();
    while (client.connected() && millis() - start < 10000) {  // 10 second timeout
      if (client.available()) {
        char c = client.read();
        // End of HTTP request
        if (c == '\n' && currentLineIsBlank) {
          // Send HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          // Send HTML page
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head><title>Arduino Server</title></head>");
          client.println("<body>");
          client.println("<h1>Hello World from Arduino!</h1>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    client.flush();  // Give the client time to receive the data
    client.stop();
    Serial.println("Client disconnected");
  }
}