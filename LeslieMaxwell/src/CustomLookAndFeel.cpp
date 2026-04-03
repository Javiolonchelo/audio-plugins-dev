// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "CustomLookAndFeel.h"
#include "BinaryData.h"
#include "common.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    int dataSize = 0;
    const void* data = BinaryData::getNamedResource("HellasFunBold_ttf", dataSize);
    if (data != nullptr)
        customTypeface = Typeface::createSystemTypefaceFor(data, static_cast<size_t>(dataSize));

    setColour(ComboBox::textColourId, Colours::whitesmoke);
    setColour(ComboBox::backgroundColourId, Colours::transparentBlack);
    setColour(ComboBox::outlineColourId, Colours::transparentBlack);
    setColour(PopupMenu::backgroundColourId, Colour(0xFF201B15));
    setColour(PopupMenu::textColourId, Colour(0xFFF1E6D0));
    setColour(PopupMenu::highlightedBackgroundColourId, Colour(0xFF5B4128));
    setColour(PopupMenu::highlightedTextColourId, Colours::white);

}

Font CustomLookAndFeel::getCustomFont(float height) const
{
    if (customTypeface != nullptr)
        return Font(customTypeface).withHeight(height);
    return Font(height, Font::bold);
}

void CustomLookAndFeel::drawRotarySlider (Graphics &g, int x, int y, int width, int height,
                                         float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, Slider &slider)
{
    const float outerRadius = jmin(width, height) * 0.42f;
    const float centreX     = x + width * 0.5f;
    const float centreY     = y + height * 0.5f;
    const float angle       = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    const float innerRadius = outerRadius * 0.55f;
    const float ringWidth   = outerRadius - innerRadius;

    // Fixed selection line is outside the knob, so leave room
    const float selectorRadius = outerRadius * 1.22f;

    // Ring rotation: numbers are painted reversed (10 where 1 would be),
    // so we rotate by +angle to make "1" align with thumb at min and "10" at max.
    const float ringRotation = angle;

    // --- Fixed selection line at 12 o'clock (background element) ---
    {
        const float lineInner = outerRadius * 1.02f;
        const float lineOuter = selectorRadius;
        g.setColour(Colour(0xFFB4B4B4));
        g.drawLine(centreX, centreY - lineInner,
                   centreX, centreY - lineOuter, 2.5f);
    }

    // --- Outer ring drop shadow (melatonin blur) ---
    {
        const float angleRad = shadowAngle * MathConstants<float>::pi / 180.0f;
        const float shadowOff = outerRadius * shadowLength;
        const int sx = static_cast<int>(std::cos(angleRad) * shadowOff);
        const int sy = static_cast<int>(std::sin(angleRad) * shadowOff);
        outerShadow.setOffset(sx, sy);
        outerShadow.setRadius(static_cast<int>(outerRadius * 0.18f));

        Path outerPath;
        outerPath.addEllipse(centreX - outerRadius, centreY - outerRadius,
                             outerRadius * 2.0f, outerRadius * 2.0f);
        outerShadow.render(g, outerPath);
    }

    // --- Outer black ring ---
    {
        ColourGradient ringGrad(Colour(0xFF3A3A3A), centreX - outerRadius, centreY - outerRadius,
                                Colour(0xFF0A0A0A), centreX + outerRadius, centreY + outerRadius, false);
        g.setGradientFill(ringGrad);
        g.fillEllipse(centreX - outerRadius, centreY - outerRadius, outerRadius * 2.0f, outerRadius * 2.0f);
    }

    // Apply rotation transform for everything on the ring
    g.saveState();
    g.addTransform(AffineTransform::rotation(ringRotation, centreX, centreY));

    // --- Numbers / sync labels on the outer ring (reversed: 10 at start, 1 at end) ---
    {
        const bool isSpeedKnob = (slider.getName() == "SPEED");
        const bool showSync = isSpeedKnob && syncMode;
        const float numberRadius = outerRadius * 0.78f;
        const float fontSize     = showSync ? outerRadius * 0.18f : outerRadius * 0.24f;

        for (int i = 1; i <= 10; ++i)
        {
            // Reversed: number 1 is placed at the end angle, 10 at the start angle
            const float frac     = (i - 1) / 9.0f;
            const float numAngle = rotaryEndAngle - frac * (rotaryEndAngle - rotaryStartAngle);

            const int divIdx = i - 1;

            g.saveState();
            g.addTransform(AffineTransform::rotation(numAngle, centreX, centreY));

            g.setColour(Colour(0xFFB4B4B4));
            g.setFont(getCustomFont(fontSize));

            const String label = showSync ? String(SYNC_LABELS[divIdx]) : String(i);
            g.drawText(label,
                       static_cast<int>(centreX - fontSize * 1.5f),
                       static_cast<int>(centreY - numberRadius - fontSize * 0.5f),
                       static_cast<int>(fontSize * 3.0f),
                       static_cast<int>(fontSize),
                       Justification::centred, false);
            g.restoreState();
        }
    }

    // --- Tick marks on top of numbers (one per number) ---
    {
        const float tickInner = outerRadius * 0.9f;
        const float tickOuter = outerRadius * 0.97f;
        g.setColour(Colour(0xFFB4B4B4).withAlpha(0.3f));

        for (int i = 1; i <= 10; ++i)
        {
            const float frac = (i - 1) / 9.0f;
            const float tickAngle = rotaryEndAngle - frac * (rotaryEndAngle - rotaryStartAngle);
            g.drawLine(centreX + std::sin(tickAngle) * tickInner,
                       centreY - std::cos(tickAngle) * tickInner,
                       centreX + std::sin(tickAngle) * tickOuter,
                       centreY - std::cos(tickAngle) * tickOuter,
                       1.0f);
        }
    }

    // --- Inner circle drop shadow (melatonin blur) ---
    {
        const float angleRad = shadowAngle * MathConstants<float>::pi / 180.0f;
        const float shadowOff = innerRadius * shadowLength;
        const int sx = static_cast<int>(std::cos(angleRad) * shadowOff);
        const int sy = static_cast<int>(std::sin(angleRad) * shadowOff);
        innerShadow.setOffset(sx, sy);
        innerShadow.setRadius(static_cast<int>(innerRadius * 0.15f));

        Path innerPath;
        innerPath.addEllipse(centreX - innerRadius, centreY - innerRadius,
                             innerRadius * 2.0f, innerRadius * 2.0f);
        innerShadow.render(g, innerPath);
    }

    // --- Inner white circle (rotates with ring) ---
    {
        ColourGradient innerGrad(Colour(0xFFB4B4B4), centreX - innerRadius * 0.5f, centreY - innerRadius * 0.5f,
                                 Colour(0xFF969696), centreX + innerRadius * 0.5f, centreY + innerRadius * 0.5f, false);
        g.setGradientFill(innerGrad);
        g.fillEllipse(centreX - innerRadius, centreY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

        g.setColour(Colour(0xFF444444));
        g.drawEllipse(centreX - innerRadius, centreY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.0f);
    }

    // --- Label in the inner circle (rotates with ring) ---
    {
        g.setColour(Colour(0xFF222222));
        g.setFont(getCustomFont(innerRadius * 0.52f));
        g.drawText(slider.getName(),
                   static_cast<int>(centreX - innerRadius), static_cast<int>(centreY - innerRadius),
                   static_cast<int>(innerRadius * 2.0f), static_cast<int>(innerRadius * 2.0f),
                   Justification::centred, false);
    }

    g.restoreState();
}

Font CustomLookAndFeel::getComboBoxFont(ComboBox &)
{
    return getCustomFont(26.0f);
}

void CustomLookAndFeel::drawComboBox(Graphics &g, int width, int height, bool isButtonDown,
                                     int buttonX, int buttonY, int buttonW, int buttonH,
                                     ComboBox &box)
{
    ignoreUnused(g, width, height, isButtonDown, buttonX, buttonY, buttonW, buttonH, box);
}

void CustomLookAndFeel::positionComboBoxText(ComboBox &, Label &label)
{
    label.setBounds(0, 0, label.getParentWidth(), label.getParentHeight());
    label.setFont(getCustomFont(20.0f));
    label.setJustificationType(Justification::centred);
}

Font CustomLookAndFeel::getTextButtonFont(TextButton &, int)
{
    return getCustomFont(26.0f);
}

void CustomLookAndFeel::drawButtonBackground(Graphics &g, Button &button, const Colour &,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    if (button.getButtonText() == "<" || button.getButtonText() == ">")
    {
        ignoreUnused(g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        return;
    }

    LookAndFeel_V4::drawButtonBackground(g, button, Colours::transparentBlack,
                                         shouldDrawButtonAsHighlighted,
                                         shouldDrawButtonAsDown);
}

void CustomLookAndFeel::drawToggleButton(Graphics &, ToggleButton &, bool, bool)
{
    // Visuals handled by editor paint() — button overlay images drawn there
}
