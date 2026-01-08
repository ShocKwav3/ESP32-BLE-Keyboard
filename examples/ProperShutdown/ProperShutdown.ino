/**
 * This example demonstrates proper BLE shutdown using end()
 * to deinitialize the BLE stack and free resources
 */
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;
bool bleActive = false;
unsigned long startTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE...");
  bleKeyboard.begin();
  bleActive = true;
  startTime = millis();
}

void loop() {
  if(bleActive) {
    if(bleKeyboard.isConnected()) {
      Serial.println("Sending 'Hello'...");
      bleKeyboard.print("Hello");
    }

    // Stop BLE after 30 seconds
    if(millis() - startTime > 30000) {
      Serial.println("Shutting down BLE...");
      bleKeyboard.end();
      bleActive = false;
      Serial.println("BLE shutdown complete. Free to use resources for other tasks.");
    }
  }

  delay(5000);
}
