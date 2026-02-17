// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "CocoKnob.h"
#include "CustomLookAndFeel.h"
#include "MyCanvas.h"
#include "PluginProcessor.h"
#include "TitleBox.h"
#include "common.h"

#if JUCE_DEBUG
#include <melatonin_inspector/melatonin_inspector.h>
#endif

class GainAudioProcessorEditor
    : public AudioProcessorEditor
    , private AudioProcessorValueTreeState::Listener
{
public:
    // Basic
    explicit GainAudioProcessorEditor(GainAudioProcessor &);
    ~GainAudioProcessorEditor() override;
    void paint(Graphics &) override;
    void resized() override;
    void parameterChanged(const String &parameterID, float newValue) override;

    // float getCurrentScale();
    // void setCurrentScale(float newScale);

private:
    // Mouse callbacks
    void mouseDoubleClick(const MouseEvent &) override;
    void mouseDrag(const MouseEvent &) override;
    void mouseUp(const MouseEvent &) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

    // Member variables
    GainAudioProcessor       &audioProcessor;
    CustomLookAndFeel         customLookAndFeel;
    std::unique_ptr<MyCanvas> myCanvas;
    std::unique_ptr<CocoKnob> knob;
    std::unique_ptr<TitleBox> titleComponent;

    Point<int>   lastCenter             = {STARTUP_CENTER, STARTUP_CENTER};
    Point<float> posWhenStartedDragging = {0.0f, 0.0f};

    // std::atomic<float> sc;

    // APVTS
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> knobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainAudioProcessorEditor)

#if JUCE_DEBUG
    melatonin::Inspector inspector { *this, false };
#endif
};
