// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include <juce_core/juce_core.h>
using namespace juce;

// Parameter IDs
static const String P_GAIN_ID   = "gain";
static const String P_X_ID      = "x";
static const String P_Y_ID      = "y";
static const String P_SIZE_ID   = "size";
static const String P_BYPASS_ID = "bypass";
static const String P_FRENCH_ID = "french";

// Parameter names
static const String P_GAIN_NAME   = "Gain";
static const String P_X_NAME      = "Position X";
static const String P_Y_NAME      = "Position Y";
static const String P_SIZE_NAME   = "Size";
static const String P_BYPASS_NAME = "Bypass";
static const String P_FRENCH_NAME = "French Mode";

// UI
static constexpr int   STARTUP_SIZE       = 500;    // px
static constexpr int   STARTUP_CENTER     = 350;    // px
static constexpr int   MIN_SIZE           = 250;    // px
static constexpr int   MAX_SIZE           = 10000;  // px
static constexpr int   TITLE_HEIGHT       = 100;    // px
static constexpr float INITIAL_MULTIPLIER = 0.5f;

// UX
static constexpr int DOUBLE_CLICK_TIMEOUT = 300;  // ms
static constexpr int REFRESH_RATE_HZ      = 60;   // Hz
