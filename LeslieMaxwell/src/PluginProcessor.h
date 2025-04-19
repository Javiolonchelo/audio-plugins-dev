// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "PluginEditor.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "common.h"

class LeslieMaxwellProcessor : public AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                               public AudioProcessorARAExtension
#endif
{
   public:
    LeslieMaxwellProcessor();
    ~LeslieMaxwellProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

    AudioProcessorEditor *createEditor() override;
    bool                  hasEditor() const override;

    const String getName() const override;

    bool   acceptsMidi() const override;
    bool   producesMidi() const override;
    bool   isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int          getNumPrograms() override;
    int          getCurrentProgram() override;
    void         setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void         changeProgramName(int index, const String &newName) override;

    void getStateInformation(MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    std::unique_ptr<AudioProcessorValueTreeState> apvts;

   private:
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeslieMaxwellProcessor)
};
