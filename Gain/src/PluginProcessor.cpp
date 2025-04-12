#include "PluginEditor.h"
#include "PluginProcessor.h"

AudioProcessorValueTreeState::ParameterLayout GainAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>(P_GAIN_ID, P_GAIN_NAME, -30.0f, 30.0f, 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(P_X_ID, P_X_NAME, 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(P_Y_ID, P_Y_NAME, 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<AudioParameterBool>(P_BYPASS_ID, P_BYPASS_NAME, false));
    return {params.begin(), params.end()};
}

GainAudioProcessor::GainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
        #endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
    #endif
                         ),
      apvts(std::make_unique<AudioProcessorValueTreeState>(*this, nullptr, Identifier("French Coconut Gain"), createParameterLayout()))
#endif
{
}

GainAudioProcessor::~GainAudioProcessor() {}

const String GainAudioProcessor::getName() const { return JucePlugin_Name; }

bool GainAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GainAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GainAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double GainAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int GainAudioProcessor::getNumPrograms() { return 1; }

int GainAudioProcessor::getCurrentProgram() { return 0; }

void GainAudioProcessor::setCurrentProgram(int index) {}

const String GainAudioProcessor::getProgramName(int index) { return {}; }

void GainAudioProcessor::changeProgramName(int index, const String &newName) {}

void GainAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void GainAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    ScopedNoDenormals noDenormals;
    const auto        numIns  = getTotalNumInputChannels();
    const auto        numOuts = getTotalNumOutputChannels();

    for (auto i = numIns; i < numOuts; ++i) buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < numIns; ++channel)
    {
        auto *channelData = buffer.getWritePointer(channel);

        // Bypass
        if (apvts->getParameter(P_BYPASS_ID)->getValue() == true)
        {
            for (int i = 0; i < buffer.getNumSamples(); ++i) { channelData[i] = 0.0f; }
        }
        // Apply gain
        else
        {
            const auto gain_dB = apvts->getParameter(P_GAIN_ID)->getValue();
            for (int i = 0; i < buffer.getNumSamples(); ++i) { channelData[i] *= Decibels::decibelsToGain(gain_dB); }
        }
    }
}

bool GainAudioProcessor::hasEditor() const { return true; }

AudioProcessorEditor *GainAudioProcessor::createEditor() { return new GainAudioProcessorEditor(*this); }

void GainAudioProcessor::getStateInformation(MemoryBlock &destData) {}

void GainAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // Use this method to restore your parameters from the saved state
    // (called after loading the editor)
    if (data != nullptr && sizeInBytes > 0)
    {
        MemoryInputStream stream(data, sizeInBytes, false);
        apvts->state = ValueTree::readFromStream(stream);
    }
}

AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new GainAudioProcessor(); }
