// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"

#include <PluginProcessor.h>
#include <juce_opengl/juce_opengl.h>

LeslieMaxwellEditor::LeslieMaxwellEditor(LeslieMaxwellProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);

    // Layout management
    setResizable(true, false);
    // setResizeLimits(MIN_SIZE, MIN_SIZE, MAX_SIZE, MAX_SIZE);
    setSize(960, 540);
    getConstrainer()->setFixedAspectRatio(static_cast<float>(STARTUP_WIDTH / STARTUP_HEIGHT));

    // Slider settings
    vcoFreqSlider  = std::make_unique<Slider>();
    vcoFreqSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    // vcoDepthSlider = std::make_unique<Slider>();
    addAndMakeVisible(*vcoFreqSlider);
    // addAndMakeVisible(*vcoDepthSlider);

    modSlider = std::make_unique<Slider>();
    modSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(*modSlider);

    // Attachments
    vcoFreqAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_FREQ_ID, *vcoFreqSlider);
    // vcoDepthAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_DEPTH_ID, *vcoDepthSlider);
    modAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_MOD_ID, *modSlider);

    // startTimerHz(RATE);

    for (int i = 0; i < NUM_FRAMES; ++i)
    {
        String name { names[i] };

        // Load only those matching our frame pattern
        if (name.startsWith("maxwell") && name.endsWith("png"))
        {
            int dataSize = 0;
            const String binaryDataName { name.replaceCharacters(".", "_")};
            const void* dataPtr = BinaryData::getNamedResource(binaryDataName.toRawUTF8(), dataSize);

            jassert(dataPtr != nullptr);
            Image         img = ImageFileFormat::loadFrom(dataPtr, dataSize);
            frames.push_back(img);
        }
        DBG("Saved asset of index" << i);
    }

    resized();
    startTimerHz(60);
}

LeslieMaxwellEditor::~LeslieMaxwellEditor()
{
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    // knob->setLookAndFeel(nullptr);
}

void LeslieMaxwellEditor::paint(Graphics &g)
{
    // Calculate title height using float maøth and convert to int to avoid narrowing warnings
    g.fillAll(Colours::black);
    // openGLContext.triggerRepaint();

    g.drawImageWithin(frames[previousFrameIndex], 0, 0, getWidth(), getHeight(), RectanglePlacement::centred);
}

void LeslieMaxwellEditor::resized()
{
    const auto height = 60;
    // Layout sliders at the top, then the OpenGL demo occupies the remaining area.
    if (vcoFreqSlider)  vcoFreqSlider->setBounds(0, 0, getWidth(), height);
    if (vcoDepthSlider) vcoDepthSlider->setBounds(0, height, getWidth(), height);
    if (modSlider)      modSlider->setBounds(0, height, getWidth(), height);
}

void LeslieMaxwellEditor::timerCallback()
{
    // audioProcessor.currentVco is in range [0, 2pi)
    previousFrameIndex = audioProcessor.vcoPhase[0] * (NUM_FRAMES - 1) / MathConstants<float>::twoPi;
    repaint();
}
