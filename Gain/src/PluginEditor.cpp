// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"

#include <PluginProcessor.h>

GainAudioProcessorEditor::GainAudioProcessorEditor(GainAudioProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);
    LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);

    // Custom canvas
    myCanvas = std::make_unique<MyCanvas>();
    myCanvas->setSize(STARTUP_SIZE, STARTUP_SIZE + TITLE_HEIGHT);
    addAndMakeVisible(*myCanvas);

    // Layout management
    setResizable(true, false);
    setResizeLimits(MIN_SIZE, MIN_SIZE, MAX_SIZE, MAX_SIZE);
    setSize(STARTUP_SIZE, STARTUP_SIZE + TITLE_HEIGHT);
    getConstrainer()->setFixedAspectRatio(static_cast<float>(STARTUP_SIZE) / (STARTUP_SIZE + TITLE_HEIGHT));

    // Knob settings
    knob = std::make_unique<CocoKnob>();
    knob->setLookAndFeel(&customLookAndFeel);
    myCanvas->addAndMakeVisible(*knob);

    // Bypass button settings
    bypassButton = std::make_unique<TextButton>();
    bypassButton->setBounds(0, TITLE_HEIGHT, 100, 100);
    bypassButton->setButtonText("Bypass");
    bypassButton->setClickingTogglesState(true);
    bypassButton->setToggleable(true);
    // addAndMakeVisible(*bypassButton);

    // Title
    titleComponent = std::make_unique<TitleBox>(*audioProcessor.apvts);
    titleComponent->setOpaque(true);
    titleComponent->setBounds(0, 0, getWidth(), TITLE_HEIGHT);
    myCanvas->addAndMakeVisible(*titleComponent);

    // Attachments
    knobAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_GAIN_ID, *knob);
    bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(*p.apvts, P_BYPASS_ID, *bypassButton);
    knob->addListener(titleComponent.get());
    audioProcessor.apvts->addParameterListener(P_GAIN_ID, this);
    audioProcessor.apvts->addParameterListener(P_SIZE_ID, this);
    audioProcessor.apvts->addParameterListener(P_X_ID, this);
    audioProcessor.apvts->addParameterListener(P_Y_ID, this);
}

GainAudioProcessorEditor::~GainAudioProcessorEditor()
{
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    knob->setLookAndFeel(nullptr);
}

void GainAudioProcessorEditor::paint(juce::Graphics &g)
{
    // Title
    // const int titleHeight = titleComponent->getNewTitleHeight();
    titleComponent->setBounds(0, 0, myCanvas->getWidth(), TITLE_HEIGHT);

    // Knob
    const auto p_size = audioProcessor.apvts->getParameter(P_SIZE_ID);
    const auto p_x    = audioProcessor.apvts->getParameter(P_X_ID);
    const auto p_y    = audioProcessor.apvts->getParameter(P_Y_ID);

    const auto size     = p_size->convertFrom0to1(p_size->getValue());
    const auto center_x = p_x->convertFrom0to1(p_x->getValue());
    const auto center_y = p_y->convertFrom0to1(p_y->getValue());

    const float newWidth  = jmax(size * myCanvas->getWidth(), 0.0f);
    const float newHeight = jmax(size * myCanvas->getHeight(), 0.0f);
    jassert(newWidth >= 0 && newHeight >= 0);

    const float x = center_x * myCanvas->getWidth() - newWidth / 2;
    const float y = center_y * myCanvas->getHeight() - newHeight / 2;

    knob->setBounds(x, y, newWidth, newHeight);
}

void GainAudioProcessorEditor::resized()
{
    const auto area = getLocalBounds();
    const auto sc   = static_cast<float>(area.getWidth()) / STARTUP_SIZE;
    myCanvas->setTransform(AffineTransform::scale(sc));
}

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
        repaint();
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
        repaint();
    }
}
void GainAudioProcessorEditor::mouseUp(const MouseEvent &event)
{
    posWhenStartedDragging = {0.0f, 0.0f};
    repaint();
}
void GainAudioProcessorEditor::mouseDown(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p_x = audioProcessor.apvts->getParameter(P_X_ID);
        const auto p_y = audioProcessor.apvts->getParameter(P_Y_ID);

        posWhenStartedDragging.x = p_x->convertFrom0to1(p_x->getValue());
        posWhenStartedDragging.y = p_y->convertFrom0to1(p_y->getValue());
    }
}
void GainAudioProcessorEditor::mouseWheelMove(const MouseEvent &, const MouseWheelDetails &mouse_wheel_details)
{
    const auto p_size         = audioProcessor.apvts->getParameter(P_SIZE_ID);
    const auto sizeMultiplier = jlimit<float>(0.0f, 1000.0f, p_size->convertFrom0to1(p_size->getValue()) + 0.2f * mouse_wheel_details.deltaY);
    p_size->setValueNotifyingHost(p_size->convertTo0to1(sizeMultiplier));
    DBG("sizeMultiplier: " << sizeMultiplier);
    repaint();
}
void GainAudioProcessorEditor::parameterChanged(const String &parameterID, float newValue) { repaint(); }
