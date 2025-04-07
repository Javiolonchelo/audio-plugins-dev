// dBob Studio 2024
//
// Hao Feng Chen Fu
// Javier Rodrigo L�pez

#pragma once

#include "CustomColours.h"
#include <JuceHeader.h>

// #include "melatonin_blur.h"

using namespace juce;

class CustomLookAndFeel : public LookAndFeel_V4
{
  public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    // void drawLabel(Graphics &, Label &) override;

    //// Sliders
    // virtual Label *createSliderTextBox(Slider &slider) override;

    // Fonts
    virtual Typeface::Ptr getTypefaceForFont(const Font &) override;
    virtual Font getLabelFont(Label &font) override;

  private:
    const float globalFontSize = 20.0f;
    const float globalFontSize2 = 12.0f;
    ColourGradient coldAndHot;
};
