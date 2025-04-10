// dBob Studio 2024
//
// Hao Feng Chen Fu
// Javier Rodrigo L�pez

#pragma once

#include "BinaryData.h"
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"

using namespace juce;

constexpr int COCO_SIZE_X = 3 * 109;
constexpr int COCO_SIZE_Y = 3 * 114;

class CustomLookAndFeel final : public LookAndFeel_V4 {
   public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    // Sliders
    void drawRotarySlider(Graphics &, int x, int y, int width, int height, float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle, Slider &) override;

    // Fonts
    Typeface::Ptr getTypefaceForFont(const Font &) override;
    Font          getLabelFont(Label &font) override;

   private:
    const float globalFontSize  = 20.0f;
    const float globalFontSize2 = 12.0f;

    std::unique_ptr<Image> coco;
    std::unique_ptr<Image> background;
};
