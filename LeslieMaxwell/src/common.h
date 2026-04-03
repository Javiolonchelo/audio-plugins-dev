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
static const String P_MODE_ID      = "mode";
static const String P_DIRECTION_ID = "direction";
static const String P_SYNC_ID      = "sync";
static const String P_PHASE_ID     = "phase_offset";
static const String P_BYPASS_ID    = "bypass";

// Parameter names
static const String P_VCO_FREQ_NAME  = "VCO Frequency";
static const String P_VCO_DEPTH_NAME = "VCO Depth";
static const String P_MODE_NAME      = "Mode";
static const String P_BYPASS_NAME    = "Bypass";

// Processing modes
enum ProcessingMode
{
    Tremolo = 0,
    TremoloPanning,
    Vibrato,
    Doppler,
    NumModes
};

// UI
static constexpr int   NUM_FRAMES           = 201;
static constexpr int   NUM_STRIPS           = 2;
static constexpr int   FRAMES_PER_STRIP[]   = { 121, 80 };

// UX
static constexpr int DOUBLE_CLICK_TIMEOUT = 300;  // ms

// DSP
static constexpr double MAX_VCO_DEPTH = 150.0f;    // ms
static constexpr float  MAX_VCO_FREQ  = 14.0f;     // Hz
static constexpr float  RAMP_LENGTH   = 0.02f;     // s

// Tempo sync divisions (knob positions 1–10)
static constexpr int   NUM_SYNC_DIVISIONS = 10;
static constexpr float SYNC_BEATS[NUM_SYNC_DIVISIONS] = {
    64.0f, 32.0f, 16.0f, 8.0f, 4.0f,       // 16 bars, 8, 4, 2, 1
    2.0f, 1.0f, 0.5f, 0.25f, 0.125f         // 1/2, 1/4, 1/8, 1/16, 1/32
};
static const char* const SYNC_LABELS[NUM_SYNC_DIVISIONS] = {
    "16", "8", "4", "2", "1",
    "1/2", "1/4", "1/8", "1/16", "1/32"
};

inline float syncDivisionFreq(float bpm, int divIndex)
{
    return bpm / (60.0f * SYNC_BEATS[divIndex]);
}

// Doppler physical model
static constexpr float SPEED_OF_SOUND    = 343.0f;   // m/s
static constexpr float HORN_RADIUS_MIN   = 0.01f;    // m — min rotation radius (depth = 0)
static constexpr float HORN_RADIUS_MAX   = 0.30f;    // m — max rotation radius (depth = max)
static constexpr float MIC_SPACING       = 0.17f;    // m — stereo mic separation (ORTF-ish)
static constexpr float MIC_DISTANCE      = 1.0f;     // m — fixed far-field mic distance

#endif  // COMMON_H
