/* Copyright 2025 M. MAD */

void setup() {
  Serial.begin(9600); // Initialize serial at 9600 baud
}

void loop() {
  // Generate simulated sensor values
  float sineWave = sin(millis() / 1000.0); // 1Hz sine wave
  float cosineWave = cos(millis() / 1000.0); // 1Hz cosine wave
  float linear = millis() % 10000 / 10000.0; // Ramp function (0-1)
  
  // Print formatted data for Serial Plotter
  Serial.print(sineWave);
  Serial.print(" ");
  Serial.print(cosineWave);
  Serial.print(" ");
  Serial.println(linear); // Last value needs println

  delay(10); // Short delay for stability
}
