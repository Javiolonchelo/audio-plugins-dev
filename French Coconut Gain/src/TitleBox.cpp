// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "TitleBox.h"

#include "BinaryData.h"

TitleBox::TitleBox(AudioProcessorValueTreeState& a) : apvts(a)
{
    // Title label
    title = std::make_unique<Label>();
    title->setEditable(false);
    title->setJustificationType(Justification::centred);
    title->setBorderSize(BorderSize<int>(10));
    title->setFont(customLookAndFeel.getCustomFont(21.0f));
    addAndMakeVisible(*title);

    // Load images
    croissantImage = ImageCache::getFromMemory(BinaryData::croissant_png, BinaryData::croissant_pngSize);
    flagImage      = ImageCache::getFromMemory(BinaryData::flag_png, BinaryData::flag_pngSize);

    // Bypass button (croissant) - bright when ON (bypass=false), dim when OFF (bypass=true)
    bypassButton = std::make_unique<ImageButton>("Bypass");
    bypassButton->setImages(false, true, true,
                            croissantImage, 1.0f, Colours::transparentBlack,
                            croissantImage, 1.0f, Colours::transparentBlack,
                            croissantImage, 0.5f, Colours::transparentBlack);
    bypassButton->setClickingTogglesState(true);
    bypassButton->setMouseCursor(MouseCursor::PointingHandCursor);
    bypassButton->addMouseListener(this, false);
    addAndMakeVisible(*bypassButton);

    // French mode button (flag) - dim when Normal (false), bright when Oh là là (true)
    frenchButton = std::make_unique<ImageButton>("French Mode");
    frenchButton->setImages(false, true, true,
                            flagImage, 0.5f, Colours::transparentBlack,
                            flagImage, 0.5f, Colours::transparentBlack,
                            flagImage, 1.0f, Colours::transparentBlack);
    frenchButton->setClickingTogglesState(true);
    frenchButton->setMouseCursor(MouseCursor::PointingHandCursor);
    frenchButton->addMouseListener(this, false);
    addAndMakeVisible(*frenchButton);

    // Labels for buttons
    bypassLabel = std::make_unique<Label>();
    bypassLabel->setText("Bypass", dontSendNotification);
    bypassLabel->setJustificationType(Justification::centred);
    bypassLabel->setFont(customLookAndFeel.getCustomFont(16.0f));
    bypassLabel->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(*bypassLabel);

    frenchLabel = std::make_unique<Label>();
    frenchLabel->setText("French", dontSendNotification);
    frenchLabel->setJustificationType(Justification::centred);
    frenchLabel->setFont(customLookAndFeel.getCustomFont(16.0f));
    frenchLabel->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(*frenchLabel);

    // Button attachments
    bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, P_BYPASS_ID, *bypassButton);
    frenchAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, P_FRENCH_ID, *frenchButton);

    // Listen to parameters for updates
    apvts.addParameterListener(P_GAIN_ID, this);
    apvts.addParameterListener(P_FRENCH_ID, this);
    apvts.addParameterListener(P_BYPASS_ID, this);

    // Initialize states
    if (const auto frenchParam = apvts.getParameter(P_FRENCH_ID)) {
        frenchState = frenchParam->getValue() > 0.5f;
        pendingFrenchMode.store(frenchState);
    }

    if (const auto bypassParam = apvts.getParameter(P_BYPASS_ID)) { bypassState = bypassParam->getValue() > 0.5f; }

    // Initialize labels based on state
    bypassLabel->setText(bypassState ? "OFF" : "ON", dontSendNotification);
    frenchLabel->setText(frenchState ? String(CharPointer_UTF8("Oh l\xc3\xa0 l\xc3\xa0!")) : "Normal", dontSendNotification);

    // Initialize title text
    if (const auto gainParam = apvts.getParameter(P_GAIN_ID)) {
        TitleBox::parameterChanged(P_GAIN_ID, gainParam->convertFrom0to1(gainParam->getValue()));
    }
}

TitleBox::~TitleBox()
{
    stopTimer();
    cancelPendingUpdate();
    bypassButton->removeMouseListener(this);
    frenchButton->removeMouseListener(this);
    apvts.removeParameterListener(P_GAIN_ID, this);
    apvts.removeParameterListener(P_FRENCH_ID, this);
    apvts.removeParameterListener(P_BYPASS_ID, this);
}

void TitleBox::parameterChanged(const String& id, float newValue)
{
    if (id == P_FRENCH_ID) {
        const bool newState = newValue > 0.5f;
        if (newState != frenchState) {
            frenchState = newState;
            // Calculate current visual angle and set up new rotation
            const float currentAngle = frenchStartAngle + (frenchTargetAngle - frenchStartAngle) * easeInOutCubic(
                                           frenchProgress);
            const float direction = frenchState ? MathConstants<float>::twoPi : -MathConstants<float>::twoPi;
            frenchStartAngle      = currentAngle;
            frenchTargetAngle     = direction; // Always end at 0 (visually)
            frenchProgress        = 0.0f;
            startTimerHz(REFRESH_RATE_HZ);
        }
        pendingFrenchMode.store(newState);
        const auto gainParam = apvts.getParameter(P_GAIN_ID);
        if (gainParam)
            pendingGainValue.store(gainParam->convertFrom0to1(gainParam->getValue()));
    } else if (id == P_BYPASS_ID) {
        const bool newState = newValue > 0.5f;
        if (newState != bypassState) {
            bypassState = newState;
            // Calculate current visual angle and set up new rotation
            const float currentAngle = bypassStartAngle + (bypassTargetAngle - bypassStartAngle) * easeInOutCubic(
                                           bypassProgress);
            const float direction = bypassState ? -MathConstants<float>::twoPi : MathConstants<float>::twoPi;
            bypassStartAngle      = currentAngle;
            bypassTargetAngle     = direction; // Always end at 0 (visually)
            bypassProgress        = 0.0f;
            startTimerHz(REFRESH_RATE_HZ);
        }
    } else {
        pendingGainValue.store(newValue);
    }
    triggerAsyncUpdate();
}

void TitleBox::handleAsyncUpdate()
{
    const float rawValue     = pendingGainValue.load();
    const bool frenchMode    = pendingFrenchMode.load();
    const float clampedValue = frenchMode ? rawValue : jlimit(-20.0f, 20.0f, rawValue);
    const float displayValue = std::abs(clampedValue) < 0.05f ? 0.0f : clampedValue;
    const String value(displayValue, 1);
    const String titleText = "French Coconut Gain: " + value.paddedLeft(' ', 5) + " dB";
    title->setText(titleText, dontSendNotification);

    // Update labels
    bypassLabel->setText(bypassState ? "OFF" : "ON", dontSendNotification);
    frenchLabel->setText(frenchState ? String(CharPointer_UTF8("Oh l\xc3\xa0 l\xc3\xa0!")) : "Normal", dontSendNotification);
}

void TitleBox::timerCallback()
{
    bool stillAnimating = false;

    if (bypassProgress < 1.0f)
    {
        bypassProgress += ANIMATION_STEP_MS / ANIMATION_DURATION_MS;
        if (bypassProgress >= 1.0f)
        {
            bypassProgress = 1.0f;
            bypassStartAngle = 0.0f;
            bypassTargetAngle = 0.0f;
        }
        else
        {
            stillAnimating = true;
        }
    }

    if (frenchProgress < 1.0f)
    {
        frenchProgress += ANIMATION_STEP_MS / ANIMATION_DURATION_MS;
        if (frenchProgress >= 1.0f)
        {
            frenchProgress = 1.0f;
            frenchStartAngle = 0.0f;
            frenchTargetAngle = 0.0f;
        }
        else
        {
            stillAnimating = true;
        }
    }

    // Update shake phase if hovering
    if (bypassHovering || frenchHovering)
    {
        shakePhase += SHAKE_SPEED * (ANIMATION_STEP_MS / 1000.0f);
        stillAnimating = true;
    }

    const float bypassAngle = bypassStartAngle + (bypassTargetAngle - bypassStartAngle) * easeInOutCubic(bypassProgress);
    const float frenchAngle = frenchStartAngle + (frenchTargetAngle - frenchStartAngle) * easeInOutCubic(frenchProgress);

    const float bypassShake = bypassHovering ? std::sin(shakePhase * MathConstants<float>::twoPi) * SHAKE_AMPLITUDE : 0.0f;
    const float frenchShake = frenchHovering ? std::sin(shakePhase * MathConstants<float>::twoPi) * SHAKE_AMPLITUDE : 0.0f;

    updateButtonTransform(bypassButton.get(), bypassAngle, bypassShake);
    updateButtonTransform(frenchButton.get(), frenchAngle, frenchShake);

    if (!stillAnimating) stopTimer();
}

void TitleBox::updateButtonTransform(ImageButton* button, float baseAngle, float shakeAngle)
{
    const auto bounds = button->getBounds();
    const float centreX = bounds.getCentreX();
    const float centreY = bounds.getCentreY();
    button->setTransform(AffineTransform::rotation(baseAngle + shakeAngle, centreX, centreY));
}

float TitleBox::easeInOutCubic(float t)
{
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

void TitleBox::mouseEnter(const MouseEvent& event)
{
    if (event.eventComponent == bypassButton.get())
    {
        bypassHovering = true;
        shakePhase = 0.0f;
        startTimerHz(REFRESH_RATE_HZ);
    }
    else if (event.eventComponent == frenchButton.get())
    {
        frenchHovering = true;
        shakePhase = 0.0f;
        startTimerHz(REFRESH_RATE_HZ);
    }
}

void TitleBox::mouseExit(const MouseEvent& event)
{
    if (event.eventComponent == bypassButton.get())
    {
        bypassHovering = false;
        updateButtonTransform(bypassButton.get(), bypassStartAngle + (bypassTargetAngle - bypassStartAngle) * easeInOutCubic(bypassProgress), 0.0f);
    }
    else if (event.eventComponent == frenchButton.get())
    {
        frenchHovering = false;
        updateButtonTransform(frenchButton.get(), frenchStartAngle + (frenchTargetAngle - frenchStartAngle) * easeInOutCubic(frenchProgress), 0.0f);
    }
}

void TitleBox::resized()
{
    const int marginX     = 12;
    const int marginY     = 10;
    const int buttonSize  = 68;
    const int labelHeight = 16;
    bypassButton->setBounds(marginX, marginY, buttonSize, buttonSize);
    bypassLabel->setBounds(0, buttonSize + marginY, buttonSize + 2 * marginX, labelHeight);
    frenchButton->setBounds(getWidth() - buttonSize - marginX, marginY, buttonSize, buttonSize);
    frenchLabel->setBounds(getWidth() - buttonSize - 2 * marginX, buttonSize + marginY, buttonSize + 2 * marginX, labelHeight);
    title->setBounds(0, 0, getWidth(), getHeight());
}

void TitleBox::paint(Graphics& g) { g.fillAll(Colours::black); }    