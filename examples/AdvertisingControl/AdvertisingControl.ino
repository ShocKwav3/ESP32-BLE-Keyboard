/**
 * This example demonstrates how to manually control BLE advertising
 * to save power or prevent new connections
 */
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE...");
  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    Serial.println("Device connected. Stopping advertising...");
    bleKeyboard.stopAdvertising();

    delay(10000);

    Serial.println("Re-enabling advertising...");
    bleKeyboard.startAdvertising();
  }

  delay(2000);
}
