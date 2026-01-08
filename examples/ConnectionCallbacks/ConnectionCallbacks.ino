/**
 * This example demonstrates how to use connection callbacks
 * to respond to BLE connection and disconnection events
 */
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

void onConnected() {
  Serial.println("BLE device connected!");
}

void onDisconnected(int reason) {
  Serial.print("BLE device disconnected. Reason: ");
  Serial.println(reason);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE with callbacks...");

  // Set callbacks before calling begin()
  bleKeyboard.setOnConnect(onConnected);
  bleKeyboard.setOnDisconnect(onDisconnected);

  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    Serial.println("Sending 'Hello'...");
    bleKeyboard.print("Hello");
    delay(5000);
  }
  delay(1000);
}
