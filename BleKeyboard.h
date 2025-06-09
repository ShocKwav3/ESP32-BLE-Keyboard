#pragma once

// Select implementation based on compile-time flag
#if defined(USE_NIMBLE)
  #include "NimbleKeyboard.h"
  using BleKeyboard = NimbleKeyboard;
#else
  #include "StandardBleKeyboard.h"
  using BleKeyboard = StandardBleKeyboard;
#endif
