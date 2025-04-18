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

CustomLookAndFeel::~CustomLookAndFeel() = default;

void CustomLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, Slider &slider)
{
    const auto cocoPtr = dynamic_cast<CocoKnob *>(&slider);
    jassert(cocoPtr != nullptr);

    const float centerX       = x + width / 2;
    const float centerY       = y + height / 2;
    const auto  rotationAngle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPosProportional;
    g.addTransform(AffineTransform::rotation(rotationAngle, centerX, centerY));

    // This prevents the image from being drawn outside the bounds of the knob
    g.addTransform(AffineTransform::scale(0.7f, 0.7f, centerX, centerY));

    g.drawImage(*coco, x, y, width, height, 0, 0, coco->getWidth(), coco->getHeight(), false);
}

// Fonts
Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const Font &)
{
    return Typeface::createSystemTypefaceFor(BinaryData::JuliaMonoExtraBoldItalic_ttf, BinaryData::JuliaMonoExtraBoldItalic_ttfSize);
}

Font CustomLookAndFeel::getLabelFont(Label &)
{
    const FontOptions fontOptions;
    const Font        labelFont(fontOptions.withHeight(globalFontSize));
    return labelFont;
}
