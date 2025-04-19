// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "common.h"
// #include "CustomLookAndFeel.h"
#include "PluginProcessor.h"

class LeslieMaxwellEditor : public AudioProcessorEditor, private Timer
{
   public:
    // Timer
    void timerCallback() override;

    // Basic
    explicit LeslieMaxwellEditor(LeslieMaxwellProcessor &);
    ~LeslieMaxwellEditor() override;
    void paint(Graphics &) override;
    void resized() override;

   private:
    // Mouse callbacks
    // void mouseDoubleClick(const MouseEvent &) override;
    // void mouseDrag(const MouseEvent &) override;
    // void mouseUp(const MouseEvent &) override;
    // void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

    // Custom functions

    // Member variables
    LeslieMaxwellProcessor &audioProcessor;
    // CustomLookAndFeel           customLookAndFeel;
    // std::unique_ptr<Image>      background;
    std::unique_ptr<TextButton> bypassButton;

    float        sizeMultiplier     = INITIAL_MULTIPLIER;
    Point<int>   offset             = {0, 0};
    Point<int>   lastCenter         = {STARTUP_CENTER, STARTUP_CENTER};
    Point<float> lastCenterRelative = {0.5f, 0.5f};

    // APVTS
    // std::unique_ptr<AudioProcessorValueTreeState>                  &apvts;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> knobAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeslieMaxwellEditor)
};
