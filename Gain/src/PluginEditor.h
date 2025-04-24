// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "common.h"
#include "CocoKnob.h"
#include "CustomLookAndFeel.h"
#include "PluginProcessor.h"
#include "TitleBox.h"
#include "MyCanvas.h"

class GainAudioProcessorEditor : public AudioProcessorEditor, private AudioProcessorValueTreeState::Listener
{
   public:
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
    void mouseDown(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

    // Member variables
    GainAudioProcessor         &audioProcessor;
    CustomLookAndFeel           customLookAndFeel;
    std::unique_ptr<MyCanvas>   myCanvas;
    std::unique_ptr<CocoKnob>   knob;
    std::unique_ptr<TitleBox>   titleComponent;
    std::unique_ptr<TextButton> bypassButton;

    Point<int>   lastCenter             = {STARTUP_CENTER, STARTUP_CENTER};
    Point<float> posWhenStartedDragging = {0.0f, 0.0f};

    // APVTS
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> knobAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainAudioProcessorEditor)
   public:
    void parameterChanged(const String &parameterID, float newValue) override;
};
