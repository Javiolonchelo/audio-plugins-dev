// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"

#include <PluginProcessor.h>

void GainAudioProcessorEditor::timerCallback() { repaint(); }

GainAudioProcessorEditor::GainAudioProcessorEditor(GainAudioProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);
    LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);
    background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    jassert(background != nullptr && background->isValid());

    // Layout management
    setResizable(true, false);
    setResizeLimits(MIN_SIZE, MIN_SIZE, MAX_SIZE, MAX_SIZE);
    setSize(STARTUP_SIZE, STARTUP_SIZE + TITLE_HEIGHT);
    getConstrainer()->setFixedAspectRatio(static_cast<float>(STARTUP_SIZE) / (STARTUP_SIZE + TITLE_HEIGHT));

    // Knob settings
    knob = std::make_unique<CocoKnob>();
    knob->setLookAndFeel(&customLookAndFeel);

    addAndMakeVisible(*knob);

    // Bypass button settings
    bypassButton = std::make_unique<TextButton>();
    bypassButton->setBounds(0, TITLE_HEIGHT, 100, 100);
    bypassButton->setButtonText("Bypass");
    bypassButton->setClickingTogglesState(true);
    bypassButton->setToggleable(true);
    // addAndMakeVisible(*bypassButton);

    // Title
    title = std::make_unique<Label>();
    title->setText("French Coconut Gain:  0.0 dB", dontSendNotification);
    title->setJustificationType(Justification::centred);
    title->setBorderSize(BorderSize<int>(10));
    const FontOptions opts(Font::getDefaultSerifFontName(), 20.0f, Font::bold);
    title->setFont(opts);
    addAndMakeVisible(*title);

    // Attachments
    knobAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_GAIN_ID, *knob);
    bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(*p.apvts, P_BYPASS_ID, *bypassButton);

    // Timer
    startTimerHz(60);  // 60 FPS
}

GainAudioProcessorEditor::~GainAudioProcessorEditor()
{
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    knob->setLookAndFeel(nullptr);
}

void GainAudioProcessorEditor::paint(juce::Graphics &g)
{
    const auto newTitleHeight = TITLE_HEIGHT * getHeight() / (STARTUP_SIZE + TITLE_HEIGHT);

    const Rectangle<int> r(0, 0, getWidth(), getHeight());
    g.setColour(Colours::black);
    g.fillRect(r);

    // Title
    const FontOptions opts(Font::getDefaultSerifFontName(), jmax(10.0f, newTitleHeight * 0.4f), Font::bold);
    title->setFont(opts);

    // Pad the value with spaces at beginning of string until it reaches 6 characters
    const String value(audioProcessor.apvts->getRawParameterValue(P_GAIN_ID)->load(), 1);
    const String titleText = "French Coconut Gain: " + value.paddedLeft(' ', 5) + " dB";
    title->setText(titleText, dontSendNotification);
    title->setBounds(0, 0, getWidth(), newTitleHeight);

    g.drawImageWithin(*background, 0, newTitleHeight, getWidth(), getHeight() - newTitleHeight, RectanglePlacement::stretchToFit, false);
    repaintMouseChanges();
}

void GainAudioProcessorEditor::resized()
{
    lastCenter = {static_cast<int>(lastCenterRelative.x * getWidth()), static_cast<int>(lastCenterRelative.y * getHeight())};
}

// MOUSE CALLBACKS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GainAudioProcessorEditor::mouseDoubleClick(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto w = static_cast<float>(getWidth());
        const auto h = static_cast<float>(getHeight());

        // Reset size and position
        sizeMultiplier     = INITIAL_MULTIPLIER;
        lastCenter         = event.getMouseDownPosition();
        lastCenterRelative = {event.getMouseDownPosition().x / w, event.getMouseDownPosition().y / h};
        offset             = {0, 0};

        // repaintMouseChanges();
    }
}

void GainAudioProcessorEditor::mouseDrag(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        offset = event.getOffsetFromDragStart();
        DBG("Drag offset: " << offset.x << ", " << offset.y);
        // repaintMouseChanges();
    }
}

void GainAudioProcessorEditor::mouseUp(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto w = static_cast<float>(getWidth());
        const auto h = static_cast<float>(getHeight());
        lastCenter += offset;
        lastCenterRelative = {lastCenter.x / w, lastCenter.y / h};
        offset             = {0, 0};
        DBG("lastCenter: " << lastCenter.x << ", " << lastCenter.y);
        DBG("lastCenterRelative: " << lastCenterRelative.x << ", " << lastCenterRelative.y);
        // repaintMouseChanges();
    }
}

void GainAudioProcessorEditor::mouseWheelMove(const MouseEvent &, const MouseWheelDetails &mouse_wheel_details)
{
    sizeMultiplier += mouse_wheel_details.deltaY * 0.1f;
    sizeMultiplier = jlimit(0.0f, 1000.0f, sizeMultiplier);
    DBG("sizeMultiplier: " << sizeMultiplier);
    // repaintMouseChanges();
}

// CUSTOM FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GainAudioProcessorEditor::repaintMouseChanges() const
{
    // if (knob == nullptr) return;

    const auto newWidth  = jmax(sizeMultiplier * getWidth(), 0.0f);
    const auto newHeight = jmax(sizeMultiplier * getHeight(), 0.0f);
    jassert(newWidth >= 0 && newHeight >= 0);

    const auto x = lastCenter.x + offset.x - newWidth / 2;
    const auto y = lastCenter.y + offset.y - newHeight / 2;

    knob->setBounds(x, y, newWidth, newHeight);
}