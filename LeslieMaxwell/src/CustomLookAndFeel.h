// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "melatonin_blur/melatonin_blur.h"

using namespace juce;

class CustomLookAndFeel final : public LookAndFeel_V4
{
   public:
    CustomLookAndFeel();

    void drawRotarySlider(Graphics &g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, Slider &slider) override;

    Font getComboBoxFont(ComboBox &) override;
    void drawComboBox(Graphics &g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      ComboBox &box) override;
    void positionComboBoxText(ComboBox &box, Label &label) override;
    Font getTextButtonFont(TextButton &, int buttonHeight) override;
    void drawButtonBackground(Graphics &g, Button &button, const Colour &backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    void drawToggleButton(Graphics &g, ToggleButton &button,
                          bool shouldDrawTickBox, bool isTickBoxTicked) override;
    Font getPopupMenuFont() override;

    static constexpr float shadowAngle  = 53.5f;  // degrees
    static constexpr float shadowLength = 0.145f; // fraction of outerRadius

    // Tempo sync state — updated by editor each frame
    bool syncMode = false;

    Font getCustomFont(float height) const;

   private:
    Typeface::Ptr customTypeface;
    melatonin::DropShadow outerShadow { Colours::black, 12, { 0, 0 } };
    melatonin::DropShadow innerShadow { Colours::black.withAlpha(0.5f), 8, { 0, 0 } };

};
