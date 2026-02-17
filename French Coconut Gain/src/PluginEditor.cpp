// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"

#include <PluginProcessor.h>

GainAudioProcessorEditor::GainAudioProcessorEditor(GainAudioProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);
    setLookAndFeel(&customLookAndFeel);

    // Custom canvas
    myCanvas = std::make_unique<MyCanvas>();
    myCanvas->setSize(STARTUP_SIZE, STARTUP_SIZE + TITLE_HEIGHT);
    addAndMakeVisible(*myCanvas);

    // Layout management
    setResizable(true, true);
    setResizeLimits(MIN_SIZE, MIN_SIZE, MAX_SIZE, MAX_SIZE);
    setSize(STARTUP_SIZE, STARTUP_SIZE + TITLE_HEIGHT);
    getConstrainer()->setFixedAspectRatio(static_cast<float>(STARTUP_SIZE) / (STARTUP_SIZE + TITLE_HEIGHT));

    // Knob settings
    knob = std::make_unique<CocoKnob>();
    knob->setLookAndFeel(&customLookAndFeel);
    myCanvas->addAndMakeVisible(*knob);

    // Title
    titleComponent = std::make_unique<TitleBox>(*p.apvts);
    titleComponent->setOpaque(true);
    titleComponent->setBounds(0, 0, getWidth(), TITLE_HEIGHT);
    myCanvas->addAndMakeVisible(*titleComponent);

    // Attachments
    knobAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_GAIN_ID, *knob);
    audioProcessor.apvts->addParameterListener(P_GAIN_ID, this);
    audioProcessor.apvts->addParameterListener(P_SIZE_ID, this);
    audioProcessor.apvts->addParameterListener(P_X_ID, this);
    audioProcessor.apvts->addParameterListener(P_Y_ID, this);
    audioProcessor.apvts->addParameterListener(P_FRENCH_ID, this);
    audioProcessor.apvts->addParameterListener(P_BYPASS_ID, this);

    // Initialize knob range based on current French mode state
    const bool isFrenchModeActive = p.apvts->getParameter(P_FRENCH_ID)->getValue() > 0.5f;
    knob->setRange(isFrenchModeActive ? -60.0f : -20.0f, isFrenchModeActive ? 60.0f : 20.0f);

    // setCurrentScale(p.lastScale);

    GainAudioProcessorEditor::parameterChanged(P_BYPASS_ID, p.apvts->getParameter(P_BYPASS_ID)->getValue());
}

GainAudioProcessorEditor::~GainAudioProcessorEditor()
{
    audioProcessor.apvts->removeParameterListener(P_GAIN_ID, this);
    audioProcessor.apvts->removeParameterListener(P_SIZE_ID, this);
    audioProcessor.apvts->removeParameterListener(P_X_ID, this);
    audioProcessor.apvts->removeParameterListener(P_Y_ID, this);
    audioProcessor.apvts->removeParameterListener(P_FRENCH_ID, this);
    audioProcessor.apvts->removeParameterListener(P_BYPASS_ID, this);

    setLookAndFeel(nullptr);
    knob->setLookAndFeel(nullptr);
}

void GainAudioProcessorEditor::paint(juce::Graphics&)
{
    // Title
    titleComponent->setBounds(0, 0, myCanvas->getWidth(), TITLE_HEIGHT);

    // Knob
    const auto p_size = audioProcessor.apvts->getParameter(P_SIZE_ID);
    const auto p_x    = audioProcessor.apvts->getParameter(P_X_ID);
    const auto p_y    = audioProcessor.apvts->getParameter(P_Y_ID);

    if (!p_size || !p_x || !p_y) return;

    const auto size     = p_size->convertFrom0to1(p_size->getValue());
    const auto center_x = p_x->convertFrom0to1(p_x->getValue());
    const auto center_y = p_y->convertFrom0to1(p_y->getValue());

    const float newWidth  = jmax(size * myCanvas->getWidth(), 0.0f);
    const float newHeight = jmax(size * myCanvas->getHeight(), 0.0f);
    jassert(newWidth >= 0 && newHeight >= 0);

    const float x = center_x * myCanvas->getWidth() - newWidth / 2;
    const float y = center_y * myCanvas->getHeight() - newHeight / 2;

    knob->setBounds(static_cast<int>(x), static_cast<int>(y), static_cast<int>(newWidth), static_cast<int>(newHeight));
}

void GainAudioProcessorEditor::resized()
{
    const auto area = getLocalBounds();
    const auto sc   = static_cast<float>(area.getWidth()) / STARTUP_SIZE;
    // audioProcessor.lastScale = sc;
    myCanvas->setTransform(AffineTransform::scale(sc));
}

// float GainAudioProcessorEditor::getCurrentScale()
// {
//     return sc;
// }

// void GainAudioProcessorEditor::setCurrentScale(float newScale)
// {
//     sc = newScale;
//     myCanvas->setTransform(AffineTransform::scale(sc));
// }

void GainAudioProcessorEditor::parameterChanged(const String& parameterId, float val)
{
    Component::SafePointer<GainAudioProcessorEditor> safeThis(this);
    MessageManager::callAsync([safeThis]() { if (safeThis != nullptr) safeThis->repaint(); });

    if (parameterId == P_FRENCH_ID)
    {
        if (val > 0.5f) {
            knob->setRange(-60.0f, 60.0f);
        } else {
            knob->setRange(-20.0f, 20.0f);
        }
    } else if (parameterId == P_BYPASS_ID){
        myCanvas->changeBackground(val > 0.5f);
    }
}


// MOUSE CALLBACKS ////////////////////////////////////////////////////////////

void GainAudioProcessorEditor::mouseDoubleClick(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p_size = audioProcessor.apvts->getParameter(P_SIZE_ID);
        const auto p_x    = audioProcessor.apvts->getParameter(P_X_ID);
        const auto p_y    = audioProcessor.apvts->getParameter(P_Y_ID);

        auto defaultSize = p_size->getDefaultValue();
        p_size->setValueNotifyingHost(defaultSize);

        // Get position relative to editor, then convert to canvas coordinates
        const auto  editorPos   = event.getEventRelativeTo(this).getPosition();
        const float sc          = static_cast<float>(getWidth()) / STARTUP_SIZE;
        const float canvasX     = static_cast<float>(editorPos.x) / sc;
        const float canvasY     = static_cast<float>(editorPos.y) / sc;
        const float normalizedX = canvasX / STARTUP_SIZE;
        const float normalizedY = canvasY / (STARTUP_SIZE + TITLE_HEIGHT);

        p_x->setValueNotifyingHost(p_x->convertTo0to1(normalizedX));
        p_y->setValueNotifyingHost(p_y->convertTo0to1(normalizedY));
        repaint();
    }
}
void GainAudioProcessorEditor::mouseDrag(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p_x = audioProcessor.apvts->getParameter(P_X_ID);
        const auto p_y = audioProcessor.apvts->getParameter(P_Y_ID);

        // Get offset relative to editor, then convert to canvas coordinates
        const auto  editorOffset = event.getEventRelativeTo(this).getOffsetFromDragStart();
        const float sc           = static_cast<float>(getWidth()) / STARTUP_SIZE;
        const float deltaX       = static_cast<float>(editorOffset.x) / sc / STARTUP_SIZE;
        const float deltaY       = static_cast<float>(editorOffset.y) / sc / (STARTUP_SIZE + TITLE_HEIGHT);

        const float newX = p_x->convertTo0to1(posWhenStartedDragging.x + deltaX);
        const float newY = p_y->convertTo0to1(posWhenStartedDragging.y + deltaY);
        p_x->setValueNotifyingHost(newX);
        p_y->setValueNotifyingHost(newY);
        repaint();
    }
}
void GainAudioProcessorEditor::mouseUp(const MouseEvent&)
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
    repaint();
}
