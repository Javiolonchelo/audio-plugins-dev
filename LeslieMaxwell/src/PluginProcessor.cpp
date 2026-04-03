// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"
#include "PluginProcessor.h"

AudioProcessorValueTreeState::ParameterLayout LeslieMaxwellProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>(P_VCO_DEPTH_ID , P_VCO_DEPTH_NAME, NormalisableRange<float>(0.0f, MAX_VCO_DEPTH, 0.0001f, 0.4f), 26.52f));
    params.push_back(std::make_unique<AudioParameterFloat>(P_VCO_FREQ_ID, P_VCO_FREQ_NAME, NormalisableRange<float>(0.0f, MAX_VCO_FREQ, 0.0001f, 0.43f), 2.79f));
    params.push_back(std::make_unique<AudioParameterChoice>(P_MODE_ID, P_MODE_NAME, StringArray{"Tremolo", "Tremolo + Panning", "Vibrato", "Doppler"}, 0));
    params.push_back(std::make_unique<AudioParameterBool>(P_DIRECTION_ID, "Direction", false));
    params.push_back(std::make_unique<AudioParameterBool>(P_SYNC_ID, "Sync", false));
    params.push_back(std::make_unique<AudioParameterFloat>(P_PHASE_ID, "Phase Offset",
        NormalisableRange<float>(0.0f, MathConstants<float>::twoPi, 0.0f), 0.0f));
    params.push_back(std::make_unique<AudioParameterBool>(P_BYPASS_ID, P_BYPASS_NAME, true));
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

    smoothedPhaseOffset.setCurrentAndTargetValue(apvts->getRawParameterValue(P_PHASE_ID)->load());
    smoothedPhaseOffset.reset(sampleRate, 0.2);

    storedSampleRate = sampleRate;

    // Initialize high shelf with "looking at us" state (0 dB gain, 16 kHz)
    auto coeffs = dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 16000.0f, 0.707f, 1.0f);
    for (int channel = 0; channel < 2; ++channel)
        highShelf[channel].coefficients = coeffs;

    setLatencySamples(0);
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
    const auto        N       = buffer.getNumSamples();
    const auto        Fs      = static_cast<float>(getSampleRate());

    const int mode = static_cast<int>(apvts->getRawParameterValue(P_MODE_ID)->load());
    const int direction = apvts->getRawParameterValue(P_DIRECTION_ID)->load() > 0.5f ? -1 : 1;
    const bool syncOn = apvts->getRawParameterValue(P_SYNC_ID)->load() > 0.5f;
    const bool usesPanning = (mode == TremoloPanning || mode == Doppler);

    // Read DAW tempo
    if (auto* playHead = getPlayHead())
    {
        if (auto pos = playHead->getPosition())
        {
            if (pos->getBpm())
                currentBpm.store(static_cast<float>(*pos->getBpm()));
        }
    }

    const float bpm = currentBpm.load();

    // Compute synced frequency if sync is on
    float syncedFreq = 0.0f;
    if (syncOn)
    {
        const float normPos = apvts->getParameter(P_VCO_FREQ_ID)->getValue();
        const int div = jlimit(0, NUM_SYNC_DIVISIONS - 1,
                               static_cast<int>(std::round(normPos * (NUM_SYNC_DIVISIONS - 1))));
        syncedFreq = syncDivisionFreq(bpm, div);
    }

    // Detect transitions that could cause clicks
    if (mode != lastMode || direction != lastDirection)
    {
        crossfadePos = 0; // restart fade-in ramp
        lastMode = mode;
        lastDirection = direction;
    }

    // Report latency only for modes using the delay line
    if (mode == Doppler)
        setLatencySamples(static_cast<int>(MIC_DISTANCE / SPEED_OF_SOUND * Fs));
    else if (mode == Vibrato)
        setLatencySamples(delayBuffer[0].getSize() / 2);
    else
        setLatencySamples(0);

    // Sync channels when not using panning so L/R stay locked
    if (!usesPanning && numIns > 1)
    {
        vcoPhase[1]     = vcoPhase[0];
        tremoloPhase[1] = tremoloPhase[0];
    }

    smoothedPhaseOffset.setTargetValue(apvts->getRawParameterValue(P_PHASE_ID)->load());

    for (int channel = 0; channel < numIns; ++channel)
    {
        // 90-degree spread centred: L = -45°, R = +45°
        const float panOffset = usesPanning ? (channel == 0 ? -MathConstants<float>::pi / 4.0f : MathConstants<float>::pi / 4.0f) : 0.0f;
        vcoFreq[channel]->setTargetValue(syncOn ? syncedFreq
                                                   : apvts->getRawParameterValue(P_VCO_FREQ_ID)->load());
        vcoDepth[channel]->setTargetValue(apvts->getRawParameterValue(P_VCO_DEPTH_ID)->load());

        auto *y = buffer.getWritePointer(channel);
        for (int n = 0; n < N; ++n)
        {
            currentVco = vcoFreq[channel]->getNextValue() * direction;
            const float userPhaseOff = (channel == 0) ? smoothedPhaseOffset.getNextValue()
                                                      : smoothedPhaseOffset.getCurrentValue();

            delayBuffer[channel].push(y[n]);
            vcoPhase[channel] = std::fmod(vcoPhase[channel] + MathConstants<float>::twoPi * currentVco / Fs + MathConstants<float>::twoPi, MathConstants<float>::twoPi);
            tremoloPhase[channel] = std::fmod(tremoloPhase[channel] + MathConstants<float>::twoPi * currentVco / Fs, MathConstants<float>::twoPi);

            const auto newDepthValue = vcoDepth[channel]->getNextValue();
            const float depthNorm    = newDepthValue / static_cast<float>(MAX_VCO_DEPTH);
            const float phaseWithOffset = vcoPhase[channel] + panOffset + userPhaseOff;
            const auto vcoOut        = newDepthValue * std::cos(phaseWithOffset) + static_cast<float>(delayBuffer[channel].getSize() / 2);
            const float tremoloGain  = 1.0f - depthNorm * (1.0f - std::cos(tremoloPhase[channel] + panOffset + userPhaseOff)) * 0.5f;

            switch (mode)
            {
                case Tremolo:
                    y[n] *= tremoloGain;
                    break;

                case Vibrato:
                    y[n] = delayBuffer[channel].get(vcoOut);
                    break;

                case TremoloPanning:
                    y[n] *= tremoloGain;
                    break;

                case Doppler:
                {
                    // Physical Doppler model: rotating source, stereo mics
                    // depthNorm scales horn rotation radius (more radius = more Doppler pitch shift)
                    const float hornR = HORN_RADIUS_MIN + depthNorm * (HORN_RADIUS_MAX - HORN_RADIUS_MIN);
                    const float micX = (channel == 0) ? -MIC_SPACING * 0.5f : MIC_SPACING * 0.5f;

                    // Source position on rotating circle (negate phase for Doppler direction)
                    const float dopplerPhase = MathConstants<float>::twoPi - (vcoPhase[channel] + userPhaseOff);
                    const float srcX = hornR * std::sin(dopplerPhase);
                    const float srcY = hornR * std::cos(dopplerPhase);

                    // Distance from source to fixed far-field mic
                    const float dx = srcX - micX;
                    const float dy = srcY - MIC_DISTANCE;
                    const float dist = std::sqrt(dx * dx + dy * dy);

                    // Reference distance (source centred, facing mic) for normalisation
                    const float refDist = MIC_DISTANCE - hornR;

                    // Propagation delay in samples
                    const float delaySamples = dist / SPEED_OF_SOUND * Fs;

                    // Amplitude: inverse distance law, normalised so closest = 1.0
                    const float amplitude = refDist / dist;

                    y[n] = delayBuffer[channel].get(delaySamples) * amplitude;
                    break;
                }
            }
        }
    }

    // High shelf modulated by source orientation — Doppler mode only
    // When source faces the mic (cos component > 0) → brighter; facing away → duller
    if (mode == Doppler)
    {
        const float depthNormEnd = apvts->getRawParameterValue(P_VCO_DEPTH_ID)->load() / static_cast<float>(MAX_VCO_DEPTH);
        const float hornR = HORN_RADIUS_MIN + depthNormEnd * (HORN_RADIUS_MAX - HORN_RADIUS_MIN);

        for (int channel = 0; channel < numIns; ++channel)
        {
            const float micX = (channel == 0) ? -MIC_SPACING * 0.5f : MIC_SPACING * 0.5f;
            const float phaseOff = smoothedPhaseOffset.getCurrentValue();
            const float dopplerPhase = MathConstants<float>::twoPi - (vcoPhase[channel] + phaseOff);
            const float srcX = hornR * std::sin(dopplerPhase);
            const float srcY = hornR * std::cos(dopplerPhase);

            // Dot product of source forward direction with source-to-mic vector
            // Normalised to [0, 1]: 1 = facing mic, 0 = facing away
            const float dx = micX - srcX;
            const float dy = MIC_DISTANCE - srcY;
            const float dist = std::sqrt(dx * dx + dy * dy);
            const float fwdX = std::sin(dopplerPhase);
            const float fwdY = std::cos(dopplerPhase);
            const float facing = (fwdX * dx + fwdY * dy) / dist * 0.5f + 0.5f;

            const float shelfGainDb = -3.0f * (1.0f - facing);
            const float shelfCutoff = 5000.0f * std::pow(3.2f, facing);
            const float shelfGainLinear = Decibels::decibelsToGain(shelfGainDb);

            highShelf[channel].coefficients = dsp::IIR::Coefficients<float>::makeHighShelf(Fs, shelfCutoff, 0.707f, shelfGainLinear);
            for (int n = 0; n < N; ++n)
                buffer.getWritePointer(channel)[n] = highShelf[channel].processSample(buffer.getReadPointer(channel)[n]);
        }
    }
    else
    {
        for (int channel = 0; channel < numIns; ++channel)
            highShelf[channel].reset();
    }

    // Apply crossfade ramp after transitions to avoid clicks
    if (crossfadePos < CROSSFADE_LEN)
    {
        for (int n = 0; n < N; ++n)
        {
            if (crossfadePos < CROSSFADE_LEN)
            {
                const float gain = static_cast<float>(crossfadePos) / static_cast<float>(CROSSFADE_LEN);
                for (int channel = 0; channel < numIns; ++channel)
                    buffer.getWritePointer(channel)[n] *= gain;
                ++crossfadePos;
            }
        }
    }
}

void LeslieMaxwellProcessor::processBlockBypassed(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    setLatencySamples(0);

    // Keep delay buffer fed so switching back is click-free
    const auto numIns = getTotalNumInputChannels();
    const auto N = buffer.getNumSamples();
    for (int channel = 0; channel < numIns; ++channel)
    {
        const auto* x = buffer.getReadPointer(channel);
        for (int n = 0; n < N; ++n)
            delayBuffer[channel].push(x[n]);
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
