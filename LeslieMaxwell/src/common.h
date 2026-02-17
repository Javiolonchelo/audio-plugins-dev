// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#ifndef COMMON_H
#define COMMON_H

#include <juce_core/juce_core.h>
using namespace juce;

// Parameter IDs
static const String P_VCO_FREQ_ID  = "vco_freq";
static const String P_VCO_DEPTH_ID = "vco_depth";
static const String P_MOD_ID       = "mod";
static const String P_BYPASS_ID    = "bypass";

// Parameter names
static const String P_VCO_FREQ_NAME  = "VCO Frequency";
static const String P_VCO_DEPTH_NAME = "VCO Depth";
static const String P_BYPASS_NAME    = "Bypass";

// UI
static constexpr int   NUM_FRAMES         = 201;
static constexpr int   STARTUP_WIDTH      = 540;    // px
static constexpr int   STARTUP_HEIGHT     = 960;    // px
static constexpr int   STARTUP_CENTER     = 350;    // px
static constexpr int   MIN_SIZE           = 250;    // px
static constexpr int   MAX_SIZE           = 10000;  // px
static constexpr int   TITLE_HEIGHT       = 50;     // px
static constexpr float INITIAL_MULTIPLIER = 0.5f;

static constexpr int PLUGIN_WIDTH  = 960;
static constexpr int PLUGIN_HEIGHT = 540;

static constexpr float KNOB_WIDTH  = PLUGIN_WIDTH / 7;
static constexpr float KNOB_HEIGHT = PLUGIN_HEIGHT / 3;

// UX
static constexpr int DOUBLE_CLICK_TIMEOUT = 300;  // ms

// DSP
static constexpr double MAX_VCO_DEPTH = 100.0f;    // ms
static constexpr float  MAX_VCO_FREQ  = 10.0f;  // Hz
static constexpr float  RAMP_LENGTH   = 0.02f;    // s

#endif  // COMMON_H
