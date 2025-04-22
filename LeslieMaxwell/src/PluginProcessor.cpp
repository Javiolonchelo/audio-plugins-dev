// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"
#include "PluginProcessor.h"

AudioProcessorValueTreeState::ParameterLayout LeslieMaxwellProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>(P_VCO_FREQ_ID, P_VCO_FREQ_NAME, NormalisableRange<float>(0.0f, MAX_VCO_FREQ, 0.0001f, 0.4f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(P_VCO_DEPTH_ID, P_VCO_DEPTH_NAME, NormalisableRange<float>(0.0f, MAX_VCO_DEPTH, 0.0001f), 0.0f));
    params.push_back(std::make_unique<AudioParameterBool>(P_BYPASS_ID, P_BYPASS_NAME, true));
    // params.push_back(std::make_unique<AudioParameterFloat>(P_X_ID, P_X_NAME, 0.0f, 1.0f, 0.5f));
    // params.push_back(std::make_unique<AudioParameterFloat>(P_Y_ID, P_Y_NAME, 0.0f, 1.0f, 0.5f));
    return {params.begin(), params.end()};
}

LeslieMaxwellProcessor::LeslieMaxwellProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
        #endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
    #endif
                         ),
      apvts(std::make_unique<AudioProcessorValueTreeState>(*this, nullptr, "French Coconut Gain", createParameterLayout()))
#endif
{
    for (int channel = 0; channel < 2; ++channel)
    {
        vcoDepth[channel] = std::make_unique<SmoothedValue<float>>();
        vcoFreq[channel]  = std::make_unique<SmoothedValue<float>>();
    }
}

LeslieMaxwellProcessor::~LeslieMaxwellProcessor() {}

const String LeslieMaxwellProcessor::getName() const { return JucePlugin_Name; }

bool LeslieMaxwellProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool LeslieMaxwellProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool LeslieMaxwellProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double LeslieMaxwellProcessor::getTailLengthSeconds() const { return 0.0; }

int LeslieMaxwellProcessor::getNumPrograms() { return 1; }

int LeslieMaxwellProcessor::getCurrentProgram() { return 0; }

void LeslieMaxwellProcessor::setCurrentProgram(int index) {}

const String LeslieMaxwellProcessor::getProgramName(int index) { return {}; }

void LeslieMaxwellProcessor::changeProgramName(int index, const String &newName) {}

void LeslieMaxwellProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    for (int channel = 0; channel < 2; ++channel)
    {
        delayBuffer[channel].prepare(static_cast<int>(2.0f * MAX_VCO_DEPTH * sampleRate / 1000.0f));
        vcoDepth[channel]->setCurrentAndTargetValue(apvts->getRawParameterValue(P_VCO_DEPTH_ID)->load());
        vcoFreq[channel]->setCurrentAndTargetValue(apvts->getRawParameterValue(P_VCO_FREQ_ID)->load());
        vcoDepth[channel]->reset(sampleRate, RAMP_LENGTH);
        vcoFreq[channel]->reset(sampleRate, RAMP_LENGTH);
    }
}

void LeslieMaxwellProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LeslieMaxwellProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    #if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
    #else
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo()) return false;

        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;
        #endif
    return true;
    #endif
}
#endif

/**
 *
 * @param buffer
 * @param midiMessages
 *
 * We must specify the depth in terms of time instead of samples, this way the depth becomes independent of the sample rate. If we don't do this, same settings
 * will results in different au
 */
void LeslieMaxwellProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    ScopedNoDenormals noDenormals;
    const auto        numIns  = getTotalNumInputChannels();
    const auto        numOuts = getTotalNumOutputChannels();
    const auto        N       = buffer.getNumSamples();
    const auto        Fs      = static_cast<float>(getSampleRate());

    setLatencySamples(static_cast<int>(2.0f * MAX_VCO_DEPTH * Fs / 1000.0f));

    for (int channel = 0; channel < numIns; ++channel)
    {
        vcoDepth[channel]->setTargetValue(apvts->getRawParameterValue(P_VCO_FREQ_ID)->load());
        vcoFreq[channel]->setTargetValue(apvts->getRawParameterValue(P_VCO_DEPTH_ID)->load());
        if (apvts->getParameter(P_BYPASS_ID)->getValue() == true)
        {
            auto *y = buffer.getWritePointer(channel);
            for (int n = 0; n < N; ++n)
            {
                delayBuffer[channel].push(y[n]);
                vcoPhase[channel] =
                    std::fmod(vcoPhase[channel] + MathConstants<float>::twoPi * vcoFreq[channel]->getNextValue() / Fs, MathConstants<float>::twoPi);

                const auto newDepthValue = vcoDepth[channel]->getNextValue();
                const auto vcoOut        = newDepthValue * std::sin(vcoPhase[channel]) + static_cast<float>(delayBuffer[channel].getSize() / 2);
                y[n]                     = delayBuffer[channel].get(vcoOut);
            }
        }
    }
}
void LeslieMaxwellProcessor::processBlockBypassed(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    setLatencySamples(2.0f * MAX_VCO_DEPTH * getSampleRate() / 1000.0f);
}

bool LeslieMaxwellProcessor::hasEditor() const { return true; }

AudioProcessorEditor *LeslieMaxwellProcessor::createEditor() { return new LeslieMaxwellEditor(*this); }

void LeslieMaxwellProcessor::getStateInformation(MemoryBlock &destData) {}

void LeslieMaxwellProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    if (data != nullptr && sizeInBytes > 0)
    {
        MemoryInputStream stream(data, sizeInBytes, false);
        apvts->state = ValueTree::readFromStream(stream);
    }
}

AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new LeslieMaxwellProcessor(); }
