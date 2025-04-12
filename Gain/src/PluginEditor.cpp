#include "PluginEditor.h"

#include <PluginProcessor.h>

GainAudioProcessorEditor::GainAudioProcessorEditor(GainAudioProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p), apvts(p.apvts)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);
    LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);
    background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    knob       = std::make_unique<CocoKnob>();
    jassert(background != nullptr && background->isValid());

    // Layout management
    setResizable(true, false);
    setResizeLimits(MIN_SIZE, MIN_SIZE, MAX_SIZE, MAX_SIZE);
    setSize(STARTUP_SIZE, STARTUP_SIZE);
    getConstrainer()->setFixedAspectRatio(1.0f);

    // Knob settings
    knobAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*apvts, P_GAIN_ID, *knob);
    knob->setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(*knob);
}

GainAudioProcessorEditor::~GainAudioProcessorEditor()
{
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    knob->setLookAndFeel(nullptr);
}

void GainAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.drawImageWithin(*background, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit, false);
}

void GainAudioProcessorEditor::resized()
{
    lastCenter = {static_cast<int>(lastCenterRelative.x * getWidth()), static_cast<int>(lastCenterRelative.y * getHeight())};
    repaintMouseChanges();
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

        repaintMouseChanges();
    }
}

void GainAudioProcessorEditor::mouseDrag(const MouseEvent &event)
{
    if (event.mods.isRightButtonDown())
    {
        offset = event.getOffsetFromDragStart();
        DBG("Drag offset: " << offset.x << ", " << offset.y);
        repaintMouseChanges();
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
        repaintMouseChanges();
    }
}

void GainAudioProcessorEditor::mouseWheelMove(const MouseEvent &, const MouseWheelDetails &mouse_wheel_details)
{
    sizeMultiplier += mouse_wheel_details.deltaY * 0.1f;
    sizeMultiplier = jlimit(0.0f, 1000.0f, sizeMultiplier);
    DBG("sizeMultiplier: " << sizeMultiplier);
    repaintMouseChanges();
}

// CUSTOM FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GainAudioProcessorEditor::repaintMouseChanges() const
{
    const auto newWidth  = jmax(sizeMultiplier * getWidth(), 0.0f);
    const auto newHeight = jmax(sizeMultiplier * getHeight(), 0.0f);
    jassert(newWidth >= 0 && newHeight >= 0);

    const auto x = lastCenter.x + offset.x - newWidth / 2;
    const auto y = lastCenter.y + offset.y - newHeight / 2;

    knob->setBounds(x, y, newWidth, newHeight);
}