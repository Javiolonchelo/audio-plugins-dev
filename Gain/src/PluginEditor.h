#pragma once

#include "CocoKnob.h"
#include "CustomLookAndFeel.h"
#include "PluginProcessor.h"
#include "juce_audio_utils/juce_audio_utils.h"

class GainAudioProcessorEditor final : public AudioProcessorEditor {
   public:
    explicit GainAudioProcessorEditor(GainAudioProcessor &);
    ~GainAudioProcessorEditor() override;

    void paint(Graphics &) override;
    void resized() override;

   private:
    GainAudioProcessor &audioProcessor;

    CustomLookAndFeel         customLookAndFeel;
    std::unique_ptr<CocoKnob> knob;
    std::unique_ptr<Image>    backgroundImage;

    float knobTextHeight = 12.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainAudioProcessorEditor)
};
