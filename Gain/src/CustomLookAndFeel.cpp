//  Studio 2024 - All rights reserved

#include "BinaryData.h"
#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    coco = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::coco_png, BinaryData::coco_pngSize));
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

void CustomLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &)
{
    if (coco->isValid())
    {
        int limit = std::min(width, height);
        int cocoWidth = limit * coco->getWidth() / static_cast<float>(coco->getHeight());
        int cocoHeight = limit * coco->getHeight() / static_cast<float>(coco->getWidth());

        float limitWidth = jmin(cocoWidth, limit) / 1.5f;
        float limitHeight = jmin(cocoHeight, limit) / 1.5f;
        g.addTransform(AffineTransform::rotation(rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPosProportional, x + width / 2.0f, y + height / 2.0f));

        g.drawImage(*coco, x + (width - limitWidth) / 2.0f, y + (height - limitHeight) / 2.0f, limitWidth, limitHeight, 0, 0, coco->getWidth(), coco->getHeight());
        g.drawRect(x + (width - limitWidth) / 2.0f, y + (height - limitHeight) / 2.0f, limitWidth, limitHeight); // Draw the border
    }
    else
    {
        g.fillAll(Colours::black);
    }
}

// Fonts
Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const Font &)
{
    return Typeface::createSystemTypefaceFor(BinaryData::lmroman12regular_otf, BinaryData::lmroman12regular_otfSize);
}

Font CustomLookAndFeel::getLabelFont(Label &label)
{
    return Font(globalFontSize);
}
