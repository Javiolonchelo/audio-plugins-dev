// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_dsp/juce_dsp.h"
#include "common.h"
#include "FractionalRingBuffer.h"

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
    void processBlockBypassed(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

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

    float currentVco{0.0f};
    float vcoPhase[2] = {0.0f, 0.0f};

    // Tempo sync state (read by editor for UI)
    std::atomic<float> currentBpm { 120.0f };

   private:static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    FractionalRingBuffer<float> delayBuffer[2];

    // High shelf filter (modulated by cat orientation)
    dsp::IIR::Filter<float> highShelf[2];
    double storedSampleRate = 44100.0;

    // VCO internal state variables
    float tremoloPhase[2] = {0.0f, 0.0f};

    std::unique_ptr<SmoothedValue<float, ValueSmoothingTypes::Linear>> vcoFreq[2];
    std::unique_ptr<SmoothedValue<float, ValueSmoothingTypes::Linear>> vcoDepth[2];
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedPhaseOffset;

    // Crossfade on mode/direction change to avoid clicks
    int lastMode = -1;
    int lastDirection = 1;
    int crossfadePos = 0;       // current position in fade-in ramp
    static constexpr int CROSSFADE_LEN = 128; // ~3ms at 44.1kHz

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeslieMaxwellProcessor)
};

