/**
 * This example demonstrates how to switch between paired devices
 * using clearBonds() to prevent auto-reconnect from the previous device.
 *
 * Use Case: You want to pair with a new device (Phone B) but the old
 * device (Phone A) keeps auto-reconnecting when you disconnect.
 *
 * Flow:
 * 1. Device pairs with Phone A and sends keystrokes
 * 2. After 15 seconds, initiates device switching
 * 3. Disconnects from Phone A
 * 4. Clears bonding info (prevents Phone A from auto-reconnecting)
 * 5. Starts advertising for Phone B to pair
 * 6. Once Phone B connects, sends keystrokes to Phone B
 */
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;
unsigned long connectionTime = 0;
bool hasSwitchedDevice = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Device Switching Example...");
  Serial.println("Pair with your first device (Phone A) now");
  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    if(connectionTime == 0) {
      connectionTime = millis();
      if(!hasSwitchedDevice) {
        Serial.println("Phone A connected!");
        Serial.println("Sending test message to Phone A...");
        bleKeyboard.println("Hello from ESP32 - Device A");
      } else {
        Serial.println("Phone B connected!");
        Serial.println("Sending test message to Phone B...");
        bleKeyboard.println("Hello from ESP32 - Device B");
        Serial.println("Device switching successful!");
      }
    }

    // After 15 seconds connected to Phone A, switch to Phone B
    if(!hasSwitchedDevice && (millis() - connectionTime > 15000)) {
      Serial.println("\n--- Switching to new device ---");
      Serial.println("Step 1: Disconnecting from Phone A...");
      bleKeyboard.disconnect();
      delay(200);

      Serial.println("Step 2: Clearing bonding info...");
      bleKeyboard.clearBonds();
      delay(200);

      Serial.println("Step 3: Starting advertising for Phone B...");
      bleKeyboard.startAdvertising();

      Serial.println("Phone A will NOT auto-reconnect.");
      Serial.println("Pair with your second device (Phone B) now");

      hasSwitchedDevice = true;
      connectionTime = 0;
    }
  } else {
    connectionTime = 0;
  }

  delay(1000);
}
