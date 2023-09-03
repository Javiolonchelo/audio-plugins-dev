/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class GainAudioProcessorEditor : public juce::AudioProcessorEditor
{
  public:
    GainAudioProcessorEditor(GainAudioProcessor &);
    ~GainAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainAudioProcessor &audioProcessor;

    float knobTextHeight = 20.0f;

    juce::Slider knob;
    juce::LookAndFeel_V4 customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainAudioProcessorEditor)
};

// TODO: Add a custom LookAndFeel class. This will allow to change the appearance of the plugin. We will reuse this
// class in future plugins.
