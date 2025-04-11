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

    setResizable(true, true);
    setResizeLimits(10, 10, 10000, 10000);
    setSize(800, 800);
    getConstrainer()->setFixedAspectRatio(1.0f);
    LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);

    knob->setRange(-30.0, 30, 0.1);
    const auto w             = static_cast<float>(getWidth());
    const auto h             = static_cast<float>(getHeight());
    knob->lastCenter         = {static_cast<int>(w / 2), static_cast<int>(h / 2)};
    knob->lastCenterRelative = {knob->lastCenter.x / w, knob->lastCenter.y / h};
    knob->setRotaryParameters(MathConstants<float>::pi * 4.0f / 3.0f, MathConstants<float>::pi * 8.0f / 3.0f, true);
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
