// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "CustomLookAndFeel.h"

#include "BinaryData.h"
#include "CocoKnob.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    coco = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::coco_png, BinaryData::coco_pngSize));
    jassert(coco != nullptr && coco->isValid());

    customTypeface = Typeface::createSystemTypefaceFor(BinaryData::JuliaMonoExtraBoldItalic_ttf, BinaryData::JuliaMonoExtraBoldItalic_ttfSize);
    jassert(customTypeface != nullptr);
}

CustomLookAndFeel::~CustomLookAndFeel() = default;

void CustomLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, Slider&)
{
    const float centerX       = static_cast<float>(x) + static_cast<float>(width) / 2;
    const float centerY       = static_cast<float>(y) + static_cast<float>(height) / 2;
    const auto  rotationAngle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPosProportional;
    g.addTransform(AffineTransform::rotation(rotationAngle, centerX, centerY));

    // This prevents the image from being drawn outside the bounds of the knob
    g.addTransform(AffineTransform::scale(0.7f, 0.7f, centerX, centerY));

    g.drawImage(*coco, x, y, width, height, 0, 0, coco->getWidth(), coco->getHeight(), false);
}

// Fonts
Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const Font &)
{
    return customTypeface;
}

Font CustomLookAndFeel::getCustomFont(float height) const
{
    return FontOptions(customTypeface).withHeight(height);
}
