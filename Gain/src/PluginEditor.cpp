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

void GainAudioProcessorEditor::resized() {}

// MOUSE CALLBACKS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GainAudioProcessorEditor::mouseDoubleClick(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p_size = audioProcessor.apvts->getParameter(P_SIZE_ID);
        const auto p_x    = audioProcessor.apvts->getParameter(P_X_ID);
        const auto p_y    = audioProcessor.apvts->getParameter(P_Y_ID);

        p_size->setValueNotifyingHost(p_size->convertTo0to1(INITIAL_MULTIPLIER));

        const auto pos = event.getMouseDownPosition();
        p_x->setValueNotifyingHost(p_x->convertTo0to1(pos.x / static_cast<float>(getWidth())));
        p_y->setValueNotifyingHost(p_y->convertTo0to1(pos.y / static_cast<float>(getHeight())));
    }
}

void GainAudioProcessorEditor::mouseDrag(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p_x = audioProcessor.apvts->getParameter(P_X_ID);
        const auto p_y = audioProcessor.apvts->getParameter(P_Y_ID);

        const auto offset = event.getOffsetFromDragStart();

        const float newX = p_x->convertTo0to1(posWhenStartedDragging.x + static_cast<float>(offset.x) / static_cast<float>(getWidth()));
        const float newY = p_y->convertTo0to1(posWhenStartedDragging.y + static_cast<float>(offset.y) / static_cast<float>(getHeight()));
        p_x->setValueNotifyingHost(newX);
        p_y->setValueNotifyingHost(newY);
    }
}

void GainAudioProcessorEditor::mouseUp(const MouseEvent &event) { posWhenStartedDragging = {0.0f, 0.0f}; }
void GainAudioProcessorEditor::mouseDown(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p_x = audioProcessor.apvts->getParameter(P_X_ID);
        const auto p_y = audioProcessor.apvts->getParameter(P_Y_ID);

        posWhenStartedDragging.x = p_x->convertFrom0to1(p_x->getValue());
        posWhenStartedDragging.y = p_y->convertFrom0to1(p_y->getValue());
    }
    AudioProcessorEditor::mouseDown(event);
}

void GainAudioProcessorEditor::mouseWheelMove(const MouseEvent &, const MouseWheelDetails &mouse_wheel_details)
{
    const auto p_size         = audioProcessor.apvts->getParameter(P_SIZE_ID);
    const auto sizeMultiplier = jlimit<float>(0.0f, 1000.0f, p_size->convertFrom0to1(p_size->getValue()) + 0.2f * mouse_wheel_details.deltaY);
    p_size->setValueNotifyingHost(p_size->convertTo0to1(sizeMultiplier));
    DBG("sizeMultiplier: " << sizeMultiplier);
}

// CUSTOM FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GainAudioProcessorEditor::repaintMouseChanges()
{
    const auto p_size = audioProcessor.apvts->getParameter(P_SIZE_ID);
    const auto p_x    = audioProcessor.apvts->getParameter(P_X_ID);
    const auto p_y    = audioProcessor.apvts->getParameter(P_Y_ID);

    const auto size     = p_size->convertFrom0to1(p_size->getValue());
    const auto center_x = p_x->convertFrom0to1(p_x->getValue());
    const auto center_y = p_y->convertFrom0to1(p_y->getValue());

    const float newWidth  = jmax(size * getWidth(), 0.0f);
    const float newHeight = jmax(size * getHeight(), 0.0f);
    jassert(newWidth >= 0 && newHeight >= 0);

    const float x = center_x * getWidth() - newWidth / 2;
    const float y = center_y * getHeight() - newHeight / 2;

    knob->setBounds(x, y, newWidth, newHeight);
}