/*
  ==============================================================================

  This file contains the basic framework code for a JUCE plugin editor.

  This file is part of Foobar.

  Foobar is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  Foobar is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Foobar. If not, see
  <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
GainAudioProcessorEditor::GainAudioProcessorEditor(GainAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    backgroundImage = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    jassert(backgroundImage != nullptr);
    jassert(backgroundImage->isValid());

    setSize(400, 400);
    LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);

    knob.setRange(-30.0, 30, 0.1);
    knob.setRotaryParameters(juce::MathConstants<float>::pi * 1.75f, juce::MathConstants<float>::pi * 3.25f, true);
    knob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    knob.setValue(0);
    knob.setDoubleClickReturnValue(true, 0.0);
    knob.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(knob);
}

GainAudioProcessorEditor::~GainAudioProcessorEditor()
{
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    knob.setLookAndFeel(nullptr);
}

//==============================================================================
void GainAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(Colours::black);
    g.drawImageWithin(*backgroundImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit, false);
}

void GainAudioProcessorEditor::resized()
{
    FlexBox f(FlexBox::Direction::column,
              FlexBox::Wrap::noWrap,
              FlexBox::AlignContent::center,
              FlexBox::AlignItems::center,
              FlexBox::JustifyContent::center);

    f.items.add(FlexItem(knob).withFlex(1.0f).withWidth(COCO_SIZE_X).withHeight(COCO_SIZE_Y));
    f.performLayout(getLocalBounds());
}
