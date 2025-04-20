// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"
#include "PluginProcessor.h"

AudioProcessorValueTreeState::ParameterLayout LeslieMaxwellProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>(P_VCO_FREQ_ID, P_VCO_FREQ_NAME, NormalisableRange<float>(0.0f, MAX_VCO_FREQ, 0.0001f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(P_VCO_DEPTH_ID, P_VCO_DEPTH_NAME, NormalisableRange<float>(0.0f, MAX_VCO_DEPTH_MS, 0.0001f), 0.0f));
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
    for (int channel = 0; channel < 2; ++channel) { delayBuffer[channel].prepare(static_cast<int>(2.0f * MAX_VCO_DEPTH_MS * sampleRate / 1000.0f)); }
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

void LeslieMaxwellProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    setLatencySamples(2 * MAX_VCO_DEPTH_MS);

    ScopedNoDenormals noDenormals;
    const auto        numIns  = getTotalNumInputChannels();
    const auto        numOuts = getTotalNumOutputChannels();
    const auto        N       = buffer.getNumSamples();

    // VCO parameters and state
    const float vcoFreq  = apvts->getRawParameterValue(P_VCO_FREQ_ID)->load();
    const float vcoDepth = apvts->getRawParameterValue(P_VCO_DEPTH_ID)->load();

    for (int channel = 0; channel < numIns; ++channel)
    {
        if (apvts->getParameter(P_BYPASS_ID)->getValue() == true)
        {
            auto *y        = buffer.getWritePointer(channel);
            float vcoPhase = 0.0f;
            for (int n = 0; n < N; ++n)
            {
                delayBuffer[channel].push(y[n]);
                vcoPhase = vcoPhase_old[channel] + MathConstants<float>::twoPi * vcoFreq * static_cast<float>(n);

                const float vcoOut = vcoDepth * (1 + std::sin(vcoPhase));
                y[n]               = delayBuffer[channel].get(vcoOut);
                // const float vcoOut = vcoDepth * (1 + std::sin(vcoPhase));
                // y[n] *= vcoOut;
            }
            vcoPhase_old[channel] = vcoPhase;
        }
    }
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
