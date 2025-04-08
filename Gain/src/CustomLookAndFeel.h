// dBob Studio 2024
//
// Hao Feng Chen Fu
// Javier Rodrigo L�pez

#pragma once

#include "BinaryData.h"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"

// #include "melatonin_blur.h"

using namespace juce;

constexpr int COCO_SIZE_X = 3 * 109;
constexpr int COCO_SIZE_Y = 3 * 114;

class CustomLookAndFeel : public LookAndFeel_V4
{
  public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    // void drawLabel(Graphics &, Label &) override;

    //// Sliders
    virtual void drawRotarySlider(Graphics &, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &) override;

    // Fonts
    virtual Typeface::Ptr getTypefaceForFont(const Font &) override;
    virtual Font getLabelFont(Label &font) override;

  private:
    const float globalFontSize = 20.0f;
    const float globalFontSize2 = 12.0f;
    ColourGradient coldAndHot;

    std::unique_ptr<Image> coco;
    std::unique_ptr<Image> background;
};
