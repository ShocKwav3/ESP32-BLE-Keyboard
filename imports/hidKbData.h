#pragma once

#include "cstdint"
#include "cstddef"
#include "pgmspace.h"
#include "HIDTypes.h"

#define KEYBOARD_ID 0x01
#define MEDIA_KEYS_ID 0x02

extern const uint8_t _asciimap[128] PROGMEM;

extern const uint8_t _hidReportDescriptor[] PROGMEM;
extern const size_t _hidReportDescriptorSize;
