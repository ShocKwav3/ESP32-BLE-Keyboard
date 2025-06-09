#pragma once

// Select strategy based on compile-time flag
#if defined(USE_NIMBLE)
  #include "NimBleKeyboard.h"
  using BleKeyboard = NimBleKeyboard;
#else
  #include "StandardBleKeyboard.h"
  using BleKeyboard = StandardBleKeyboard;
#endif
