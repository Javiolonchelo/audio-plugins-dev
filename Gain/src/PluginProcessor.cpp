#include "PluginEditor.h"
#include "PluginProcessor.h"

GainAudioProcessor::GainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                         .withInput("Input", AudioChannelSet::stereo(), true)
        #endif
                         .withOutput("Output", AudioChannelSet::stereo(), true)
    #endif
      )
#endif
{
}

GainAudioProcessor::~GainAudioProcessor() {}

const String GainAudioProcessor::getName() const { return JucePlugin_Name; }

bool GainAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GainAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GainAudioProcessor::isMidiEffect() const {
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

void GainAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GainAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
    #if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
    #else
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;
        #endif
    return true;
    #endif
}
#endif

void GainAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) {
    ScopedNoDenormals noDenormals;
    auto              totalNumInputChannels  = getTotalNumInputChannels();
    auto              totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto *channelData = buffer.getWritePointer(channel);

        // ..do something to the data...
    }
}

bool GainAudioProcessor::hasEditor() const { return true; }

AudioProcessorEditor *GainAudioProcessor::createEditor() { return new GainAudioProcessorEditor(*this); }

void GainAudioProcessor::getStateInformation(MemoryBlock &destData) {}

void GainAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {}

AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new GainAudioProcessor(); }
