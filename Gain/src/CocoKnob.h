//
// Created by javiolonchelo on 4/8/2025.
//

#pragma once

#include "juce_audio_utils/juce_audio_utils.h"

using namespace juce;

class CocoKnob : public Slider
{
   public:
    // void paint(Graphics &) override;
    CocoKnob();

    void mouseDoubleClick(const MouseEvent &) override;
    void mouseDrag(const MouseEvent &) override;
    void mouseDown(const MouseEvent &) override;
    void mouseUp(const MouseEvent &) override;
    void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

   private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CocoKnob)
};
