#include "PluginEditor.h"

#include <PluginProcessor.h>

#include <juce_graphics/fonts/harfbuzz/hb-ot-shaper-use-machine.hh>

GainAudioProcessorEditor::GainAudioProcessorEditor(GainAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    backgroundImage =
        std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    knob = std::make_unique<CocoKnob>();
    jassert(backgroundImage != nullptr);
    jassert(backgroundImage->isValid());

    setSize(400, 400);
    LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);

    knob->setRange(-30.0, 30, 0.1);
    knob->lastCenter = {getWidth() / 2, getHeight() / 2};
    knob->setRotaryParameters(MathConstants<float>::pi * 4.0f / 3.0f, MathConstants<float>::pi * 8.0f / 3.0f,
                              true);  // From
    knob->setSliderStyle(Slider::RotaryVerticalDrag);
    knob->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    knob->setValue(0);
    knob->setDoubleClickReturnValue(true, 0.0);
    knob->setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(*knob);
}

GainAudioProcessorEditor::~GainAudioProcessorEditor() {
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    knob->setLookAndFeel(nullptr);
}

void GainAudioProcessorEditor::paint(juce::Graphics &g) {
    g.fillAll(Colours::black);
    g.drawImageWithin(*backgroundImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit, false);
}

void GainAudioProcessorEditor::resized() { knob->setBounds(0, 0, getWidth(), getHeight()); }
