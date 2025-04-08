/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "CustomLookAndFeel.h"
#include "PluginProcessor.h"
#include "CocoKnob.h"
#include "juce_audio_utils/juce_audio_utils.h"

//==============================================================================
/**
 */
class GainAudioProcessorEditor : public AudioProcessorEditor
{
  public:
    explicit GainAudioProcessorEditor(GainAudioProcessor &);
    ~GainAudioProcessorEditor() override;

    //==============================================================================
    void paint(Graphics &) override;
    void resized() override;

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainAudioProcessor &audioProcessor;

    float knobTextHeight = 12.0f;

    CustomLookAndFeel customLookAndFeel;
    std::unique_ptr<Image> backgroundImage;
    CocoKnob knob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainAudioProcessorEditor)
};

// TODO: Add a custom LookAndFeel class. This will allow to change the appearance of the plugin. We will reuse this
// class in future plugins.
