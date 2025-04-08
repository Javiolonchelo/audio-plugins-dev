//
// Created by javiolonchelo on 4/8/2025.
//

#pragma once

#include "juce_audio_utils/juce_audio_utils.h"

using namespace juce;

class CocoKnob:public Slider
{
  public:
    // ~CocoKnob() override;
    // void valueChanged() override;
    void paint(Graphics &) override;
    // void resized() override;
    // void mouseDown(const MouseEvent &) override;
    // void mouseUp(const MouseEvent &) override;
    void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

    // void mouseDrag(const MouseEvent &) override;
    // void modifierKeysChanged(const ModifierKeys &) override;
    // void mouseMove(const MouseEvent &) override;
    // void mouseExit(const MouseEvent &) override;
    // void mouseEnter(const MouseEvent &) override;

    float sizeMultiplier = 1.0f;
};
