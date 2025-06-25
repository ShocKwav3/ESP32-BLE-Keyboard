#include <BleKeyboard.h>

BleKeyboard bleKeyboard("DeviceName", "Manufacturer", 50); // Name, Manufacturer, Battery level

void setup() {
  Serial.begin(460800);
  Serial.println("Starting BLE");

  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    delay(1000);

    Serial.println("Scrolling left...");
    bleKeyboard.mouseMove(0, 0, 0, -3); // horizontal scrool

    delay(1000);

    Serial.println("Scrolling right...");
    bleKeyboard.mouseMove(0, 0, 0, 3); // horizontal scrool

    delay(1000);

    Serial.println("Scrolling down...");
    bleKeyboard.mouseMove(0, 0, -3, 0); // vertical scrool down

    delay(1000);

    Serial.println("Scrolling up...");
    bleKeyboard.mouseMove(0, 0, 3, 0); // vertical scrool up
  }

  Serial.println("Waiting 3 seconds...");
  delay(3000);
}
