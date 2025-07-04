#pragma once

#include "NimBLEDevice.h"
#include "NimBLEHIDDevice.h"
#include "NimBLECharacteristic.h"
#include "NimBLEAdvertising.h"
#include "NimBLEServer.h"
#include "Print.h"
#include "pgmspace.h"
#include "HIDTypes.h"
#include "esp_timer.h"

#define KEYBOARD_ID 0x01
#define MEDIA_KEYS_ID 0x02
#define MOUSE_ID 0x03
#define SHIFT 0x80

// HID Report Types
typedef struct {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
} KeyReport;

typedef struct
{
  uint8_t buttons; // Bitfield for button states (e.g., bit 0 for left, bit 1 for right)
  int8_t x;        // Relative X movement
  int8_t y;        // Relative Y movement
  int8_t wheel;    // Relative vertical wheel movement
  int8_t hWheel;  // Relative horizontal wheel movement
} MouseReport;

// ASCII to HID map
const uint8_t _asciimap[128] PROGMEM = {
	0x00,             // NUL
	0x00,             // SOH
	0x00,             // STX
	0x00,             // ETX
	0x00,             // EOT
	0x00,             // ENQ
	0x00,             // ACK
	0x00,             // BEL
	0x2a,			  // BS		Backspace
	0x2b,			  // TAB	Tab
	0x28,			  // LF		Enter
	0x00,             // VT
	0x00,             // FF
	0x00,             // CR
	0x00,             // SO
	0x00,             // SI
	0x00,             // DEL
	0x00,             // DC1
	0x00,             // DC2
	0x00,             // DC3
	0x00,             // DC4
	0x00,             // NAK
	0x00,             // SYN
	0x00,             // ETB
	0x00,             // CAN
	0x00,             // EM
	0x00,             // SUB
	0x00,             // ESC
	0x00,             // FS
	0x00,             // GS
	0x00,             // RS
	0x00,             // US

	0x2c,		   //  ' '
	0x1e|SHIFT,	   // !
	0x34|SHIFT,	   // "
	0x20|SHIFT,    // #
	0x21|SHIFT,    // $
	0x22|SHIFT,    // %
	0x24|SHIFT,    // &
	0x34,          // '
	0x26|SHIFT,    // (
	0x27|SHIFT,    // )
	0x25|SHIFT,    // *
	0x2e|SHIFT,    // +
	0x36,          // ,
	0x2d,          // -
	0x37,          // .
	0x38,          // /
	0x27,          // 0
	0x1e,          // 1
	0x1f,          // 2
	0x20,          // 3
	0x21,          // 4
	0x22,          // 5
	0x23,          // 6
	0x24,          // 7
	0x25,          // 8
	0x26,          // 9
	0x33|SHIFT,    // :
	0x33,          // ;
	0x36|SHIFT,    // <
	0x2e,          // =
	0x37|SHIFT,      // >
	0x38|SHIFT,      // ?
	0x1f|SHIFT,      // @
	0x04|SHIFT,      // A
	0x05|SHIFT,      // B
	0x06|SHIFT,      // C
	0x07|SHIFT,      // D
	0x08|SHIFT,      // E
	0x09|SHIFT,      // F
	0x0a|SHIFT,      // G
	0x0b|SHIFT,      // H
	0x0c|SHIFT,      // I
	0x0d|SHIFT,      // J
	0x0e|SHIFT,      // K
	0x0f|SHIFT,      // L
	0x10|SHIFT,      // M
	0x11|SHIFT,      // N
	0x12|SHIFT,      // O
	0x13|SHIFT,      // P
	0x14|SHIFT,      // Q
	0x15|SHIFT,      // R
	0x16|SHIFT,      // S
	0x17|SHIFT,      // T
	0x18|SHIFT,      // U
	0x19|SHIFT,      // V
	0x1a|SHIFT,      // W
	0x1b|SHIFT,      // X
	0x1c|SHIFT,      // Y
	0x1d|SHIFT,      // Z
	0x2f,          // [
	0x31,          // bslash
	0x30,          // ]
	0x23|SHIFT,    // ^
	0x2d|SHIFT,    // _
	0x35,          // `
	0x04,          // a
	0x05,          // b
	0x06,          // c
	0x07,          // d
	0x08,          // e
	0x09,          // f
	0x0a,          // g
	0x0b,          // h
	0x0c,          // i
	0x0d,          // j
	0x0e,          // k
	0x0f,          // l
	0x10,          // m
	0x11,          // n
	0x12,          // o
	0x13,          // p
	0x14,          // q
	0x15,          // r
	0x16,          // s
	0x17,          // t
	0x18,          // u
	0x19,          // v
	0x1a,          // w
	0x1b,          // x
	0x1c,          // y
	0x1d,          // z
	0x2f|SHIFT,    // {
	0x31|SHIFT,    // |
	0x30|SHIFT,    // }
	0x35|SHIFT,    // ~
	0			   // DEL
};

const uint8_t KEY_LEFT_CTRL = 0x80;
const uint8_t KEY_LEFT_SHIFT = 0x81;
const uint8_t KEY_LEFT_ALT = 0x82;
const uint8_t KEY_LEFT_GUI = 0x83;
const uint8_t KEY_RIGHT_CTRL = 0x84;
const uint8_t KEY_RIGHT_SHIFT = 0x85;
const uint8_t KEY_RIGHT_ALT = 0x86;
const uint8_t KEY_RIGHT_GUI = 0x87;

const uint8_t KEY_UP_ARROW = 0xDA;
const uint8_t KEY_DOWN_ARROW = 0xD9;
const uint8_t KEY_LEFT_ARROW = 0xD8;
const uint8_t KEY_RIGHT_ARROW = 0xD7;
const uint8_t KEY_BACKSPACE = 0xB2;
const uint8_t KEY_TAB = 0xB3;
const uint8_t KEY_RETURN = 0xB0;
const uint8_t KEY_ESC = 0xB1;
const uint8_t KEY_INSERT = 0xD1;
const uint8_t KEY_PRTSC = 0xCE;
const uint8_t KEY_DELETE = 0xD4;
const uint8_t KEY_PAGE_UP = 0xD3;
const uint8_t KEY_PAGE_DOWN = 0xD6;
const uint8_t KEY_HOME = 0xD2;
const uint8_t KEY_END = 0xD5;
const uint8_t KEY_CAPS_LOCK = 0xC1;
const uint8_t KEY_F1 = 0xC2;
const uint8_t KEY_F2 = 0xC3;
const uint8_t KEY_F3 = 0xC4;
const uint8_t KEY_F4 = 0xC5;
const uint8_t KEY_F5 = 0xC6;
const uint8_t KEY_F6 = 0xC7;
const uint8_t KEY_F7 = 0xC8;
const uint8_t KEY_F8 = 0xC9;
const uint8_t KEY_F9 = 0xCA;
const uint8_t KEY_F10 = 0xCB;
const uint8_t KEY_F11 = 0xCC;
const uint8_t KEY_F12 = 0xCD;
const uint8_t KEY_F13 = 0xF0;
const uint8_t KEY_F14 = 0xF1;
const uint8_t KEY_F15 = 0xF2;
const uint8_t KEY_F16 = 0xF3;
const uint8_t KEY_F17 = 0xF4;
const uint8_t KEY_F18 = 0xF5;
const uint8_t KEY_F19 = 0xF6;
const uint8_t KEY_F20 = 0xF7;
const uint8_t KEY_F21 = 0xF8;
const uint8_t KEY_F22 = 0xF9;
const uint8_t KEY_F23 = 0xFA;
const uint8_t KEY_F24 = 0xFB;

const uint8_t KEY_NUM_0 = 0xEA;
const uint8_t KEY_NUM_1 = 0xE1;
const uint8_t KEY_NUM_2 = 0xE2;
const uint8_t KEY_NUM_3 = 0xE3;
const uint8_t KEY_NUM_4 = 0xE4;
const uint8_t KEY_NUM_5 = 0xE5;
const uint8_t KEY_NUM_6 = 0xE6;
const uint8_t KEY_NUM_7 = 0xE7;
const uint8_t KEY_NUM_8 = 0xE8;
const uint8_t KEY_NUM_9 = 0xE9;
const uint8_t KEY_NUM_SLASH = 0xDC;
const uint8_t KEY_NUM_ASTERISK = 0xDD;
const uint8_t KEY_NUM_MINUS = 0xDE;
const uint8_t KEY_NUM_PLUS = 0xDF;
const uint8_t KEY_NUM_ENTER = 0xE0;
const uint8_t KEY_NUM_PERIOD = 0xEB;

typedef uint8_t MediaKeyReport[2];

// Media Keys
const MediaKeyReport KEY_MEDIA_NEXT_TRACK = {1, 0};
const MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK = {2, 0};
const MediaKeyReport KEY_MEDIA_STOP = {4, 0};
const MediaKeyReport KEY_MEDIA_PLAY_PAUSE = {8, 0};
const MediaKeyReport KEY_MEDIA_MUTE = {16, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_UP = {32, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_DOWN = {64, 0};
const MediaKeyReport KEY_MEDIA_WWW_HOME = {128, 0};
const MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER = {0, 1};
const MediaKeyReport KEY_MEDIA_CALCULATOR = {0, 2};
const MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS = {0, 4};
const MediaKeyReport KEY_MEDIA_WWW_SEARCH = {0, 8};
const MediaKeyReport KEY_MEDIA_WWW_STOP = {0, 16};
const MediaKeyReport KEY_MEDIA_WWW_BACK = {0, 32};
const MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION = {0, 64};
const MediaKeyReport KEY_MEDIA_EMAIL_READER = {0, 128};

static const uint8_t _hidReportDescriptor[] = {
  USAGE_PAGE(1),      0x01,          // USAGE_PAGE (Generic Desktop Ctrls)
  USAGE(1),           0x06,          // USAGE (Keyboard)
  COLLECTION(1),      0x01,          // COLLECTION (Application)
  // ------------------------------------------------- Keyboard
  REPORT_ID(1),       KEYBOARD_ID,   //   REPORT_ID (1)
  USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0xE0,          //   USAGE_MINIMUM (0xE0)
  USAGE_MAXIMUM(1),   0xE7,          //   USAGE_MAXIMUM (0xE7)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1), 0x01,          //   Logical Maximum (1)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  REPORT_COUNT(1),    0x08,          //   REPORT_COUNT (8)
  HIDINPUT(1),        0x02,          //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 1 byte (Reserved)
  REPORT_SIZE(1),     0x08,          //   REPORT_SIZE (8)
  HIDINPUT(1),        0x01,          //   INPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x05,          //   REPORT_COUNT (5) ; 5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  USAGE_PAGE(1),      0x08,          //   USAGE_PAGE (LEDs)
  USAGE_MINIMUM(1),   0x01,          //   USAGE_MINIMUM (0x01) ; Num Lock
  USAGE_MAXIMUM(1),   0x05,          //   USAGE_MAXIMUM (0x05) ; Kana
  HIDOUTPUT(1),       0x02,          //   OUTPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x01,          //   REPORT_COUNT (1) ; 3 bits (Padding)
  REPORT_SIZE(1),     0x03,          //   REPORT_SIZE (3)
  HIDOUTPUT(1),       0x01,          //   OUTPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x06,          //   REPORT_COUNT (6) ; 6 bytes (Keys)
  REPORT_SIZE(1),     0x08,          //   REPORT_SIZE(8)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM(0)
  LOGICAL_MAXIMUM(1), 0x65,          //   LOGICAL_MAXIMUM(0x65) ; 101 keys
  USAGE_PAGE(1),      0x07,          //   USAGE_PAGE (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0x00,          //   USAGE_MINIMUM (0)
  USAGE_MAXIMUM(1),   0x65,          //   USAGE_MAXIMUM (0x65)
  HIDINPUT(1),        0x00,          //   INPUT (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0),                 // END_COLLECTION
  // ------------------------------------------------- Media Keys
  USAGE_PAGE(1),      0x0C,          // USAGE_PAGE (Consumer)
  USAGE(1),           0x01,          // USAGE (Consumer Control)
  COLLECTION(1),      0x01,          // COLLECTION (Application)
  REPORT_ID(1),       MEDIA_KEYS_ID, //   REPORT_ID (3)
  USAGE_PAGE(1),      0x0C,          //   USAGE_PAGE (Consumer)
  LOGICAL_MINIMUM(1), 0x00,          //   LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1), 0x01,          //   LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),     0x01,          //   REPORT_SIZE (1)
  REPORT_COUNT(1),    0x10,          //   REPORT_COUNT (16)
  USAGE(1),           0xB5,          //   USAGE (Scan Next Track)     ; bit 0: 1
  USAGE(1),           0xB6,          //   USAGE (Scan Previous Track) ; bit 1: 2
  USAGE(1),           0xB7,          //   USAGE (Stop)                ; bit 2: 4
  USAGE(1),           0xCD,          //   USAGE (Play/Pause)          ; bit 3: 8
  USAGE(1),           0xE2,          //   USAGE (Mute)                ; bit 4: 16
  USAGE(1),           0xE9,          //   USAGE (Volume Increment)    ; bit 5: 32
  USAGE(1),           0xEA,          //   USAGE (Volume Decrement)    ; bit 6: 64
  USAGE(2),           0x23, 0x02,    //   Usage (WWW Home)            ; bit 7: 128
  USAGE(2),           0x94, 0x01,    //   Usage (My Computer) ; bit 0: 1
  USAGE(2),           0x92, 0x01,    //   Usage (Calculator)  ; bit 1: 2
  USAGE(2),           0x2A, 0x02,    //   Usage (WWW fav)     ; bit 2: 4
  USAGE(2),           0x21, 0x02,    //   Usage (WWW search)  ; bit 3: 8
  USAGE(2),           0x26, 0x02,    //   Usage (WWW stop)    ; bit 4: 16
  USAGE(2),           0x24, 0x02,    //   Usage (WWW back)    ; bit 5: 32
  USAGE(2),           0x83, 0x01,    //   Usage (Media sel)   ; bit 6: 64
  USAGE(2),           0x8A, 0x01,    //   Usage (Mail)        ; bit 7: 128
  HIDINPUT(1),        0x02,          //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0),                 // END_COLLECTION
  // ------------------------------------------------- Mouse
  USAGE_PAGE(1),      0x01,          // USAGE_PAGE (Generic Desktop Ctrls)
  USAGE(1),           0x02,          // USAGE (Mouse)
  COLLECTION(1),      0x01,          // COLLECTION (Application)
  REPORT_ID(1),       MOUSE_ID,      //   REPORT_ID (0x03 for Mouse)
  USAGE(1),           0x01,          //   USAGE (Pointer)
  COLLECTION(1),      0x00,          //   COLLECTION (Physical)
  USAGE_PAGE(1),      0x09,          //     USAGE_PAGE (Button)
  USAGE_MINIMUM(1),   0x01,          //     USAGE_MINIMUM (Button 1)
  USAGE_MAXIMUM(1),   0x03,          //     USAGE_MAXIMUM (Button 3)
  LOGICAL_MINIMUM(1), 0x00,          //     LOGICAL_MINIMUM (0)
  LOGICAL_MAXIMUM(1), 0x01,          //     LOGICAL_MAXIMUM (1)
  REPORT_SIZE(1),     0x01,          //     REPORT_SIZE (1)
  REPORT_COUNT(1),    0x03,          //     REPORT_COUNT (3) ; 3 buttons
  HIDINPUT(1),        0x02,          //     INPUT (Data,Var,Abs)
  REPORT_COUNT(1),    0x01,          //     REPORT_COUNT (1) ; 5 bits padding
  REPORT_SIZE(1),     0x05,          //     REPORT_SIZE (5)
  HIDINPUT(1),        0x01,          //     INPUT (Const,Array,Abs) ; padding (changed from 0x03 to 0x01 for clarity)

  USAGE_PAGE(1),      0x01,          //     USAGE_PAGE (Generic Desktop Ctrls)
  USAGE(1),           0x30,          //     USAGE (X)
  USAGE(1),           0x31,          //     USAGE (Y)
  USAGE(1),           0x38,          //     USAGE (Wheel) // Vertical scroll wheel
  LOGICAL_MINIMUM(1), 0x81,          //     LOGICAL_MINIMUM (-127) // Signed byte range
  LOGICAL_MAXIMUM(1), 0x7F,          //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),     0x08,          //     REPORT_SIZE (8) ; 8 bits for X, Y, Wheel
  REPORT_COUNT(1),    0x03,          //     REPORT_COUNT (3) ; X, Y, Wheel
  HIDINPUT(1),        0x06,          //     INPUT (Data,Var,Rel) ; Relative movement

  USAGE_PAGE(1),      0x0C,          //     USAGE PAGE (Consumer Devices)
  USAGE(2),           0x38, 0x02,    //     USAGE (AC Pan) / Horizontal Scroll Wheel (0x0C 0x0238)
  LOGICAL_MINIMUM(1), 0x81,          //     LOGICAL_MINIMUM (-127)
  LOGICAL_MAXIMUM(1), 0x7F,          //     LOGICAL_MAXIMUM (127)
  REPORT_SIZE(1),     0x08,          //     REPORT_SIZE (8)
  REPORT_COUNT(1),    0x01,          //     REPORT_COUNT (1)
  HIDINPUT(1),        0x06,          //     INPUT (Data, Var, Rel)

  END_COLLECTION(0),                 //   END_COLLECTION (Physical)
  END_COLLECTION(0)                  // END_COLLECTION (Application)
};

class NimBleKeyboard : public Print, public NimBLEServerCallbacks, public NimBLECharacteristicCallbacks {
private:
    NimBLEHIDDevice* hid;
    NimBLECharacteristic* inputKeyboard;
    NimBLECharacteristic* outputKeyboard;
    NimBLECharacteristic* inputMediaKeys;
	BLECharacteristic* inputMouse;
    NimBLEAdvertising* advertising;
    KeyReport _keyReport;
    MediaKeyReport _mediaKeyReport;
	MouseReport _mouseReport;
    String deviceName;
    String deviceManufacturer;
    uint8_t batteryLevel;
    bool connected = false;
    uint32_t _delay_ms = 7;
    uint16_t vid = 0x05ac;
    uint16_t pid = 0x820a;
    uint16_t version = 0x0210;
    void delay_ms(uint64_t ms);

public:
    NimBleKeyboard(String deviceName = "ESP32 Keyboard", String deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
    void begin(void);
    void end(void);
    void sendReport(KeyReport* keys);
    void sendReport(MediaKeyReport* keys);
	void sendReport(MouseReport* mouse);
    size_t press(uint8_t k);
    size_t press(const MediaKeyReport k);
    size_t release(uint8_t k);
    size_t release(const MediaKeyReport k);
    size_t write(uint8_t c);
    size_t write(const MediaKeyReport c);
    size_t write(const uint8_t *buffer, size_t size);
	void mouseMove(int8_t x, int8_t y, int8_t wheel = 0, int8_t hWheel = 0);
    void releaseAll(void);
    bool isConnected(void);
    void setBatteryLevel(uint8_t level);
    void setName(String deviceName);
    void setDelay(uint32_t ms);
    void set_vendor_id(uint16_t vid);
    void set_product_id(uint16_t pid);
    void set_version(uint16_t version);

protected:
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo & connInfo) override;
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo & connInfo, int reason) override;
    void onWrite(NimBLECharacteristic* me, NimBLEConnInfo & connInfo) override;
};
