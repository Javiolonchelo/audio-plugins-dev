//  Studio 2024 - All rights reserved

#include "CustomLookAndFeel.h"
#include "BinaryData.h"

#include "CocoKnob.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    jassert(background != nullptr);
    jassert(background->isValid());

    coco = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::coco_png, BinaryData::coco_pngSize));
    jassert(coco != nullptr);
    jassert(coco->isValid());
}

CustomLookAndFeel::~CustomLookAndFeel()
= default;

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

void CustomLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &slider)
{
    if (dynamic_cast<CocoKnob *>(&slider) != nullptr) {

        const auto w = static_cast<float>(width);
        const auto h = static_cast<float>(height);
        const auto w_coco = static_cast<float>(coco->getWidth()) * dynamic_cast<CocoKnob *>(&slider)->sizeMultiplier;
        const auto h_coco = static_cast<float>(coco->getHeight()) * dynamic_cast<CocoKnob *>(&slider)->sizeMultiplier;

        g.addTransform(AffineTransform::rotation(rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPosProportional, static_cast<float>(x) + w / 2.0f, static_cast<float>(y) + h / 2.0f));

        // Modify the bright of coco image before drawing it

        g.drawImage(*coco, x + (width - w_coco) / 2.0f, y + (height - h_coco) / 2.0f, w_coco, h_coco, 0, 0, coco->getWidth(), coco->getHeight());
        g.drawRect(x + (width - w_coco) / 2.0f, y + (height - h_coco) / 2.0f, w_coco, h_coco); // Draw the border
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
