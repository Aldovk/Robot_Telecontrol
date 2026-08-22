#include <Wire.h>

// --- Config ---
const int S0 = 6; 
const int S1 = 7; 
const int S2 = 8; 
const int ADC_PIN = A0;

// 7-Segment Display Pins
const int DISPLAY_A = 2; 
const int DISPLAY_B = 4; 
const int DISPLAY_C = 3; 
const int DEBUG_LED = 5;

// --- Calibration (Your Exact Values) ---
const int SENSOR_MIN = 595; 
const int SENSOR_MAX = 680; 

void setup() {
  // MUX Pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  
  // Display Pins
  pinMode(DISPLAY_A, OUTPUT);
  pinMode(DISPLAY_B, OUTPUT);
  pinMode(DISPLAY_C, OUTPUT);
  
  // ADC Pin
  pinMode(ADC_PIN, INPUT);
  
  // Debug LED
  pinMode(DEBUG_LED, OUTPUT);
  
  // Initialize Serial (Tx/Rx used for OpenRB)
  Serial.begin(115200);
  Serial.println("Sensor 0 Ready: 595-680 Range");
  
  // Show "0" on display
  displayNumber(0);
  
  // Flash LED to confirm boot
  digitalWrite(DEBUG_LED, HIGH);
  delay(200);
  digitalWrite(DEBUG_LED, LOW);
  delay(500);
}

void loop() {
  // Select Channel 0 (Address 0 = Binary 000)
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  
  delayMicroseconds(20); // MUX settle time
  
  // Read Raw Value
  int rawValue = analogRead(ADC_PIN);
  
  // Scale to 0-1023
  int scaledValue = map(rawValue, SENSOR_MIN, SENSOR_MAX, 0, 1023);
  
  // Clamp to ensure it stays within 0-1023
  if (scaledValue < 0) scaledValue = 0;
  if (scaledValue > 1023) scaledValue = 1023;
  
  // Send: Header (0xAA) + Low Byte + High Byte
  Serial.write(0xAA);
  Serial.write(lowByte(scaledValue));
  Serial.write(highByte(scaledValue));
  
  // Optional: Flash LED on every update
  digitalWrite(DEBUG_LED, HIGH);
  delay(50);
  digitalWrite(DEBUG_LED, LOW);
  
  delay(1000); // Wait 1 second
}

// --- Function to Display Number on 7-segment ---
void displayNumber(int num) {
  // Mapping for SN74LS47N (Common Cathode)
  // DISPLAY_A = Pin 2 (Input C, Value 4)
  // DISPLAY_B = Pin 4 (Input A, Value 1)
  // DISPLAY_C = Pin 3 (Input B, Value 2)
  
  digitalWrite(DISPLAY_A, (num & 1) ? HIGH : LOW); 
  digitalWrite(DISPLAY_B, (num & 2) ? HIGH : LOW); 
  digitalWrite(DISPLAY_C, (num & 4) ? HIGH : LOW); 
}