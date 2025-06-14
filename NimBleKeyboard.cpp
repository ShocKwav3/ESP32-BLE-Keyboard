// NimbleKeyboard.cpp
#if defined(USE_NIMBLE)

#include "NimBleKeyboard.h"

NimBleKeyboard::NimBleKeyboard(String name, String manufacturer, uint8_t battery)
    : deviceName(name), deviceManufacturer(manufacturer), batteryLevel(battery) {}

void NimBleKeyboard::begin() {
    NimBLEDevice::init(deviceName.c_str());
    NimBLEServer* pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(this);

    hid = new NimBLEHIDDevice(pServer);
    inputKeyboard = hid->getInputReport(KEYBOARD_ID);
    outputKeyboard = hid->getOutputReport(KEYBOARD_ID);
    inputMediaKeys = hid->getInputReport(MEDIA_KEYS_ID);

    outputKeyboard->setCallbacks(this);

    hid->setManufacturer(deviceManufacturer.c_str());
    hid->setPnp(0x02, vid, pid, version);
    hid->setHidInfo(0x00, 0x01);

    NimBLEDevice::setSecurityAuth(true, true, true);

    hid->setReportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
    hid->startServices();

    advertising = pServer->getAdvertising();
    advertising->setAppearance(HID_KEYBOARD);
    advertising->addServiceUUID(hid->getHidService()->getUUID());
    advertising->enableScanResponse(false);
    advertising->start();

    hid->setBatteryLevel(batteryLevel);
}

void NimBleKeyboard::end() {}

bool NimBleKeyboard::isConnected() {
    return connected;
}

void NimBleKeyboard::setBatteryLevel(uint8_t level) {
    batteryLevel = level;

    if (hid) {
        hid->setBatteryLevel(batteryLevel);
    }
}

void NimBleKeyboard::setName(String name) {
    deviceName = name;
}

void NimBleKeyboard::setDelay(uint32_t ms) {
    _delay_ms = ms;
}

void NimBleKeyboard::set_vendor_id(uint16_t v) {
    vid = v;
}

void NimBleKeyboard::set_product_id(uint16_t p) {
    pid = p;
}

void NimBleKeyboard::set_version(uint16_t v) {
    version = v;
}

void NimBleKeyboard::sendReport(KeyReport* keys) {
    if (isConnected()) {
        inputKeyboard->setValue((uint8_t*)keys, sizeof(KeyReport));
        inputKeyboard->notify();

        delay_ms(_delay_ms);
    }
}

void NimBleKeyboard::sendReport(MediaKeyReport* keys) {
    if (isConnected()) {
        inputMediaKeys->setValue((uint8_t*)keys, sizeof(MediaKeyReport));
        inputMediaKeys->notify();

        delay_ms(_delay_ms);
    }
}

size_t NimBleKeyboard::press(uint8_t k) {
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers |= (1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		k = pgm_read_byte(_asciimap + k);
		if (!k) {
			setWriteError();
			return 0;
		}
		if (k & 0x80) {						// it's a capital letter or other character reached with shift
			_keyReport.modifiers |= 0x02;	// the left shift modifier
			k &= 0x7F;
		}
	}

	// Add k to the key report only if it's not already present
	// and if there is an empty slot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
		_keyReport.keys[2] != k && _keyReport.keys[3] != k &&
		_keyReport.keys[4] != k && _keyReport.keys[5] != k) {

		for (i=0; i<6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = k;
				break;
			}
		}
		if (i == 6) {
			setWriteError();
			return 0;
		}
	}
	sendReport(&_keyReport);
	return 1;
}

size_t NimBleKeyboard::press(const MediaKeyReport k) {
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);

    mediaKeyReport_16 |= k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

	sendReport(&_mediaKeyReport);
	return 1;
}

size_t NimBleKeyboard::release(uint8_t k) {
	uint8_t i;
	if (k >= 136) {			// it's a non-printing key (not a modifier)
		k = k - 136;
	} else if (k >= 128) {	// it's a modifier key
		_keyReport.modifiers &= ~(1<<(k-128));
		k = 0;
	} else {				// it's a printing key
		k = pgm_read_byte(_asciimap + k);
		if (!k) {
			return 0;
		}
		if (k & 0x80) {							// it's a capital letter or other character reached with shift
			_keyReport.modifiers &= ~(0x02);	// the left shift modifier
			k &= 0x7F;
		}
	}

	// Test the key report to see if k is present.  Clear it if it exists.
	// Check all positions in case the key is present more than once (which it shouldn't be)
	for (i=0; i<6; i++) {
		if (0 != k && _keyReport.keys[i] == k) {
			_keyReport.keys[i] = 0x00;
		}
	}

	sendReport(&_keyReport);
	return 1;
}

size_t NimBleKeyboard::release(const MediaKeyReport k) {
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);
    mediaKeyReport_16 &= ~k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

	sendReport(&_mediaKeyReport);
	return 1;
}

void NimBleKeyboard::releaseAll() {
	_keyReport.keys[0] = 0;
	_keyReport.keys[1] = 0;
	_keyReport.keys[2] = 0;
	_keyReport.keys[3] = 0;
	_keyReport.keys[4] = 0;
	_keyReport.keys[5] = 0;
	_keyReport.modifiers = 0;
    _mediaKeyReport[0] = 0;
    _mediaKeyReport[1] = 0;
	sendReport(&_keyReport);
	sendReport(&_mediaKeyReport);
}

size_t NimBleKeyboard::write(uint8_t c) {
	uint8_t p = press(c);  // Keydown
	release(c);            // Keyup
	return p;              // just return the result of press() since release() almost always returns 1
}

size_t NimBleKeyboard::write(const MediaKeyReport c) {
	uint16_t p = press(c);  // Keydown
	release(c);            // Keyup
	return p;              // just return the result of press() since release() almost always returns 1
}

size_t NimBleKeyboard::write(const uint8_t *buffer, size_t size) {
	size_t n = 0;
	while (size--) {
		if (*buffer != '\r') {
			if (write(*buffer)) {
			  n++;
			} else {
			  break;
			}
		}
		buffer++;
	}
	return n;
}

void NimBleKeyboard::delay_ms(uint64_t ms) {
  uint64_t m = esp_timer_get_time();
  if(ms){
    uint64_t e = (m + (ms * 1000));
    if(m > e){ //overflow
        while(esp_timer_get_time() > e) { }
    }
    while(esp_timer_get_time() < e) {}
  }
}

void NimBleKeyboard::onConnect(NimBLEServer* pServer, NimBLEConnInfo & connInfo) {
    connected = true;
}

void NimBleKeyboard::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo & connInfo, int reason) {
    connected = false;
    advertising->start();
}

void NimBleKeyboard::onWrite(NimBLECharacteristic* me, NimBLEConnInfo & connInfo) {
    // Not used
}

#endif
