// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "common.h"
#include "CocoKnob.h"
#include "CustomLookAndFeel.h"
#include "PluginProcessor.h"

class GainAudioProcessorEditor : public AudioProcessorEditor, private Timer
{
   public:
    // Timer
    void timerCallback() override;

    // Basic
    explicit GainAudioProcessorEditor(GainAudioProcessor &);
    ~GainAudioProcessorEditor() override;
    void paint(Graphics &) override;
    void resized() override;

   private:
    // Mouse callbacks
    void mouseDoubleClick(const MouseEvent &) override;
    void mouseDrag(const MouseEvent &) override;
    void mouseUp(const MouseEvent &) override;
    void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

    // Custom functions
    void repaintMouseChanges() const;

    // Member variables
    GainAudioProcessor         &audioProcessor;
    CustomLookAndFeel           customLookAndFeel;
    std::unique_ptr<CocoKnob>   knob;
    std::unique_ptr<Image>      background;
    std::unique_ptr<TextButton> bypassButton;
    std::unique_ptr<Label>      title;

    float        sizeMultiplier     = INITIAL_MULTIPLIER;
    Point<int>   offset             = {0, 0};
    Point<int>   lastCenter         = {STARTUP_CENTER, STARTUP_CENTER};
    Point<float> lastCenterRelative = {0.5f, 0.5f};

    // APVTS
    // std::unique_ptr<AudioProcessorValueTreeState>                  &apvts;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> knobAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainAudioProcessorEditor)
};
