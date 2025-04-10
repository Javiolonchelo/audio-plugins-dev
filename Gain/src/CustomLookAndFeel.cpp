//  Studio 2024 - All rights reserved

#include "CustomLookAndFeel.h"

#include "BinaryData.h"
#include "CocoKnob.h"

CustomLookAndFeel::CustomLookAndFeel() {
    background =
        std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    jassert(background != nullptr);
    jassert(background->isValid());

    coco = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::coco_png, BinaryData::coco_pngSize));
    jassert(coco != nullptr);
    jassert(coco->isValid());
}

CustomLookAndFeel::~CustomLookAndFeel() = default;

void CustomLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
                                         float rotaryStartAngle, float rotaryEndAngle, Slider &slider) {
    const auto cocoPtr = dynamic_cast<CocoKnob *>(&slider);
    jassert(cocoPtr != nullptr);

    const auto newWidth  = jmax(cocoPtr->sizeMultiplier * width, 0.0f);
    const auto newHeight = jmax(cocoPtr->sizeMultiplier * height, 0.0f);
    jassert(newWidth >= 0 && newHeight >= 0);

    const auto newX = cocoPtr->lastCenter.x + cocoPtr->offset.x - newWidth / 2;
    const auto newY = cocoPtr->lastCenter.y + cocoPtr->offset.y - newHeight / 2;

    const auto newCenterX = newX + newWidth / 2;
    const auto newCenterY = newY + newHeight / 2;

    const auto rotationAngle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPosProportional;

    g.addTransform(AffineTransform::rotation(rotationAngle, newCenterX, newCenterY));
    g.drawImage(*coco, newX, newY, newWidth, newHeight, 0, 0, coco->getWidth(), coco->getHeight(), false);
}

// Fonts
Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const Font &) {
    return Typeface::createSystemTypefaceFor(BinaryData::lmroman12regular_otf, BinaryData::lmroman12regular_otfSize);
}

Font CustomLookAndFeel::getLabelFont(Label &) {
    const FontOptions fontOptions;
    const Font        labelFont(fontOptions.withHeight(globalFontSize));
    return labelFont;
}
