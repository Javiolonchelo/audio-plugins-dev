//  Studio 2024 - All rights reserved

#include <JuceHeader.h>

#include "BinaryData.h"
#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
}

CustomLookAndFeel::~CustomLookAndFeel()
{
}

// void CustomLookAndFeel::drawLabel(Graphics &g, Label &label)
//{
//     g.setColour(Colours::white);
//     // g.setFont(UltraFunnyFont.withHeight(globalFontSize));
//     g.drawFittedText(label.getText(), label.getLocalBounds(), Justification::centred, 1);
// }

// Label *CustomLookAndFeel::createSliderTextBox(Slider &slider)
//{
//     auto *textBox = new Label();
//     textBox->setFont(Font(400.0f));
//     textBox->setJustificationType(Justification::centred);
//     textBox->setText(slider.getTextFromValue(slider.getValue()), dontSendNotification);
//     textBox->setEditable(true, true, false);
//     textBox->setColour(Label::textColourId, Colours::white);
//     textBox->setColour(Label::backgroundColourId, Colours::transparentBlack);
//     return textBox;
// }

// Fonts
Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const Font &)
{
    return Typeface::createSystemTypefaceFor(BinaryData::lmroman12regular_otf, BinaryData::lmroman12regular_otfSize);
}

Font CustomLookAndFeel::getLabelFont(Label &label)
{
    return Font(globalFontSize);
}
