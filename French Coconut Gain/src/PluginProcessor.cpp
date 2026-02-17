// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"
#include "PluginProcessor.h"

AudioProcessorValueTreeState::ParameterLayout GainAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID{P_GAIN_ID, 1}, P_GAIN_NAME, -60.0f, 60.0f, 0.0f));
    params.push_back(std::make_unique<AudioParameterBool>(ParameterID{P_BYPASS_ID, 1}, P_BYPASS_NAME, false));
    params.push_back(std::make_unique<AudioParameterBool>(ParameterID{P_FRENCH_ID, 1}, P_FRENCH_NAME, false, AudioParameterBoolAttributes().withMeta(true)));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID{P_X_ID, 1}, P_X_NAME, NormalisableRange<float>(-1.0f, 2.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID{P_Y_ID, 1}, P_Y_NAME, NormalisableRange<float>(-1.0f, 2.0f, 0.001f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID{P_SIZE_ID, 1}, P_SIZE_NAME, NormalisableRange<float>(0.0f, 4.0f, 0.01f), INITIAL_MULTIPLIER));
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
      apvts(std::make_unique<AudioProcessorValueTreeState>(*this, nullptr, "FrenchCoconutGain", createParameterLayout()))
#endif
{
    apvts->addParameterListener(P_FRENCH_ID, this);
}

GainAudioProcessor::~GainAudioProcessor()
{
    apvts->removeParameterListener(P_FRENCH_ID, this);
}

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

void GainAudioProcessor::setCurrentProgram(int) {}

const String GainAudioProcessor::getProgramName(int) { return {}; }

void GainAudioProcessor::changeProgramName(int, const String &) {}

void GainAudioProcessor::prepareToPlay(double sampleRate, int) { gain.reset(sampleRate, 0.02); }

void GainAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    const auto in     = layouts.getMainInputChannelSet();
    const auto out    = layouts.getMainOutputChannelSet();
    const auto mono   = AudioChannelSet::mono();
    const auto stereo = AudioChannelSet::stereo();

    return (in == mono   && out == mono) 
        || (in == mono   && out == stereo)
        || (in == stereo && out == stereo);
}
#endif

void GainAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    ScopedNoDenormals noDenormals;
    const auto numIns     = getTotalNumInputChannels();
    const auto numOuts    = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();
    const bool bypassed   = apvts->getParameter(P_BYPASS_ID)->getValue() > 0.5f;
    const bool frenchMode = apvts->getParameter(P_FRENCH_ID)->getValue() > 0.5f;

    float gainDb = apvts->getRawParameterValue(P_GAIN_ID)->load();
    if (!frenchMode) {
        gainDb = jlimit(-20.0f, 20.0f, gainDb);
    }
    gain.setTargetValue(gainDb);

    for (auto i = numIns; i < numOuts; ++i) {
        buffer.clear(i, 0, numSamples);
    }

    if (bypassed) return;

    for (int channel = 0; channel < numIns; ++channel)
    {
        auto *y = buffer.getWritePointer(channel);
        for (int n = 0; n < numSamples; ++n) {
            y[n] *= Decibels::decibelsToGain(gain.getNextValue(), -120.0f);
        }
    }
}

bool GainAudioProcessor::hasEditor() const { return true; }

AudioProcessorEditor *GainAudioProcessor::createEditor() { return new GainAudioProcessorEditor(*this); }

void GainAudioProcessor::getStateInformation(MemoryBlock &destData)
{
    auto state = apvts->copyState();
    // state.setProperty("lastScale", lastScale, nullptr);
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GainAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    apvts->replaceState(ValueTree::fromXml(*xml));
}

void GainAudioProcessor::parameterChanged(const String &parameterID, float)
{
    if (parameterID == P_FRENCH_ID)
    {
        const auto p_gain = apvts->getParameter(P_GAIN_ID);
        if (!p_gain) return;

        auto gainValue_dB = p_gain->convertFrom0to1(p_gain->getValue());
        gainValue_dB      = jlimit(-20.0f, 20.0f, gainValue_dB);
        p_gain->setValueNotifyingHost(p_gain->convertTo0to1(gainValue_dB));
    }
}

AudioProcessorParameter* GainAudioProcessor::getBypassParameter() const
{
    return apvts->getParameter(P_BYPASS_ID);
}

AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new GainAudioProcessor(); }
