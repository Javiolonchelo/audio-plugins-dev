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
GainAudioProcessorEditor::GainAudioProcessorEditor(GainAudioProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(250, 200);

    // Add a custom font

    // Set the custom font as the default font
    juce::Font customFont(juce::Typeface::createSystemTypefaceFor(BinaryData::NeverMindHandRegular_ttf,
                                                                  BinaryData::NeverMindHandRegular_ttfSize));

    customLookAndFeel.setDefaultSansSerifTypeface(customFont.getTypefacePtr());

    juce::LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(knob);
    knob.setSliderStyle(juce::Slider::Rotary);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, knobTextHeight);
    knob.setTextValueSuffix(" dB");
    knob.setRange(-60.0, 30, 0.1);
    knob.setValue(0);
}

GainAudioProcessorEditor::~GainAudioProcessorEditor()
{
}

//==============================================================================
void GainAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(30.0f);
    juce::Rectangle<int> titleArea(0, 0, getWidth(), getHeight() - knobTextHeight);
    g.drawFittedText("Gain", titleArea, juce::Justification::centred, 1);
}

void GainAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int margin = 5;
    knob.setBounds(margin, margin, getWidth() - 2 * margin, getHeight() - 2 * margin);
}
