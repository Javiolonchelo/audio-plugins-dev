//
// Created by javiolonchelo on 4/8/2025.
//

#pragma once

#include "juce_audio_utils/juce_audio_utils.h"

using namespace juce;

class CocoKnob final : public Slider {
   public:
    void mouseDoubleClick(const MouseEvent &) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

    float      sizeMultiplier = 0.3f;
    Point<int> offset         = {0, 0};
    Point<int> lastCenter     = {0, 0};
};
