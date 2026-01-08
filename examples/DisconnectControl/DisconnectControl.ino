/**
 * This example demonstrates how to manually disconnect
 * a connected BLE device
 */
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;
unsigned long connectionTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE...");
  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    if(connectionTime == 0) {
      connectionTime = millis();
      Serial.println("Device connected!");
    }

    // Disconnect after 10 seconds
    if(millis() - connectionTime > 10000) {
      Serial.println("Disconnecting device...");
      bleKeyboard.disconnect();
      connectionTime = 0;
    }
  } else {
    connectionTime = 0;
  }

  delay(1000);
}
