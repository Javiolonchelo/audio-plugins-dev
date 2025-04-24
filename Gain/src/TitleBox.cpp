//
// Created by javiolonchelo on 23/04/2025.
//

#include "TitleBox.h"

TitleBox::TitleBox(AudioProcessorValueTreeState& apvtsFromParent) : apvts(apvtsFromParent)
{
    title = std::make_unique<Label>();
    title->setEditable(false);
    // title->onTextChange = [this]() { apvts.getParameter(P_GAIN_ID)->setValue(title->getText().getFloatValue()); };
    title->setText("First try", dontSendNotification);
    title->setJustificationType(Justification::centred);
    title->setBorderSize(BorderSize<int>(10));
    const FontOptions opts(Font::getDefaultSerifFontName(), TITLE_HEIGHT * 0.4f, Font::bold);
    title->setFont(opts);
    title->setBounds(0, 0, getWidth(), getHeight());
    addAndMakeVisible(*title);
}
void TitleBox::sliderValueChanged(Slider* s)
{
    const String value(s->getValue(), 1);
    const String titleText = "French Coconut Gain: " + value.paddedLeft(' ', 5) + " dB";
    title->setText(titleText, dontSendNotification);
}
void TitleBox::resized() { title->setBounds(0, 0, getWidth(), getHeight()); }
void TitleBox::paint(Graphics& g) { g.fillAll(Colours::black); }