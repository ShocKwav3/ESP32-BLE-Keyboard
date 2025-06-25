#include "StandardBleKeyboard.h"

StandardBleKeyboard::StandardBleKeyboard(String name, String manufacturer, uint8_t battery)
    : deviceName(name), deviceManufacturer(manufacturer), batteryLevel(battery) {}

void StandardBleKeyboard::begin() {
    BLEDevice::init(deviceName);
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    hid = new BLEHIDDevice(pServer);
    inputKeyboard = hid->inputReport(KEYBOARD_ID);
    outputKeyboard = hid->outputReport(KEYBOARD_ID);
    inputMediaKeys = hid->inputReport(MEDIA_KEYS_ID);
	inputMouse = hid->inputReport(MOUSE_ID);

    outputKeyboard->setCallbacks(this);

    hid->manufacturer()->setValue(deviceManufacturer);
    hid->pnp(0x02, vid, pid, version);
    hid->hidInfo(0x00, 0x01);

    BLESecurity* pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
    hid->startServices();

    advertising = pServer->getAdvertising();
    advertising->setAppearance(HID_KEYBOARD);
    advertising->addServiceUUID(hid->hidService()->getUUID());
    advertising->setScanResponse(false);
    advertising->start();

    hid->setBatteryLevel(batteryLevel);
}

void StandardBleKeyboard::end() {}

bool StandardBleKeyboard::isConnected() {
    return connected;
}

void StandardBleKeyboard::setBatteryLevel(uint8_t level) {
    batteryLevel = level;

    if (hid) {
        hid->setBatteryLevel(batteryLevel);
    }
}

void StandardBleKeyboard::setName(String name) {
    deviceName = name;
}
void StandardBleKeyboard::set_vendor_id(uint16_t v) {
    vid = v;
}
void StandardBleKeyboard::set_product_id(uint16_t p) {
    pid = p;
}
void StandardBleKeyboard::set_version(uint16_t v) {
    version = v;
}

void StandardBleKeyboard::sendReport(KeyReport* keys) {
    if (isConnected()) {
        inputKeyboard->setValue((uint8_t*)keys, sizeof(KeyReport));
        inputKeyboard->notify();
    }
}

void StandardBleKeyboard::sendReport(MediaKeyReport* keys) {
    if (isConnected()) {
        inputMediaKeys->setValue((uint8_t*)keys, sizeof(MediaKeyReport));
        inputMediaKeys->notify();
    }
}

void StandardBleKeyboard::sendReport(MouseReport* mouse)
{
	if (this->isConnected())
	{
		inputMouse->setValue((uint8_t*)mouse, sizeof(MouseReport));
		inputMouse->notify();
	}
}

void StandardBleKeyboard::mouseMove(int8_t x, int8_t y, int8_t wheel) {
    _mouseReport.x = x;
    _mouseReport.y = y;
    _mouseReport.wheel = wheel;

    sendReport(&_mouseReport);

    _mouseReport.x = 0;
    _mouseReport.y = 0;
    _mouseReport.wheel = 0;
}

// press() adds the specified key (printing, non-printing, or modifier)
// to the persistent key report and sends the report.  Because of the way
// USB HID works, the host acts like the key remains pressed until we
// call release(), releaseAll(), or otherwise clear the report and resend.
size_t StandardBleKeyboard::press(uint8_t k) {
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

size_t StandardBleKeyboard::press(const MediaKeyReport k) {
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);

    mediaKeyReport_16 |= k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

	sendReport(&_mediaKeyReport);
	return 1;
}

// release() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t StandardBleKeyboard::release(uint8_t k) {
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

size_t StandardBleKeyboard::release(const MediaKeyReport k) {
    uint16_t k_16 = k[1] | (k[0] << 8);
    uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);
    mediaKeyReport_16 &= ~k_16;
    _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
    _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

	sendReport(&_mediaKeyReport);
	return 1;
}

void StandardBleKeyboard::releaseAll() {
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

size_t StandardBleKeyboard::write(uint8_t c) {
	uint8_t p = press(c);  // Keydown
	release(c);            // Keyup
	return p;              // just return the result of press() since release() almost always returns 1
}

size_t StandardBleKeyboard::write(const MediaKeyReport c) {
	uint16_t p = press(c);  // Keydown
	release(c);            // Keyup
	return p;              // just return the result of press() since release() almost always returns 1
}

size_t StandardBleKeyboard::write(const uint8_t *buffer, size_t size) {
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

void StandardBleKeyboard::onConnect(BLEServer* pServer) {
    connected = true;
}

void StandardBleKeyboard::onDisconnect(BLEServer* pServer) {
    connected = false;
    advertising->start();
}

void StandardBleKeyboard::onWrite(BLECharacteristic* me) {
    // Not used
}
