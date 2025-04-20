// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"

#include <PluginProcessor.h>

void LeslieMaxwellEditor::timerCallback() { repaint(); }

LeslieMaxwellEditor::LeslieMaxwellEditor(LeslieMaxwellProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);
    // LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);
    // background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    // jassert(background != nullptr && background->isValid());

    // Layout management
    setResizable(true, false);
    setResizeLimits(MIN_SIZE, MIN_SIZE, MAX_SIZE, MAX_SIZE);
    setSize(STARTUP_SIZE, STARTUP_SIZE + TITLE_HEIGHT);
    getConstrainer()->setFixedAspectRatio(static_cast<float>(STARTUP_SIZE) / (STARTUP_SIZE + TITLE_HEIGHT));

    // Slider settings
    vcoFreqSlider  = std::make_unique<Slider>();
    vcoDepthSlider = std::make_unique<Slider>();
    addAndMakeVisible(*vcoFreqSlider);
    addAndMakeVisible(*vcoDepthSlider);

    // Bypass button settings
    bypassButton = std::make_unique<TextButton>();
    bypassButton->setBounds(0, TITLE_HEIGHT, 100, 100);
    bypassButton->setButtonText("Bypass");
    bypassButton->setClickingTogglesState(true);
    bypassButton->setToggleable(true);
    // addAndMakeVisible(*bypassButton);

    // Attachments
    vcoFreqAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_FREQ_ID, *vcoFreqSlider);
    vcoDepthAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_DEPTH_ID, *vcoDepthSlider);
    bypassAttachment   = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(*p.apvts, P_BYPASS_ID, *bypassButton);

    // Timer
    startTimerHz(60);  // 60 FPS
}

LeslieMaxwellEditor::~LeslieMaxwellEditor()
{
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    // knob->setLookAndFeel(nullptr);
}

void LeslieMaxwellEditor::paint(juce::Graphics &g)
{
    const auto newTitleHeight = TITLE_HEIGHT * getHeight() / (STARTUP_SIZE + TITLE_HEIGHT);

    const Rectangle<int> r(0, 0, getWidth(), getHeight());
    g.setColour(Colours::black);
    g.fillRect(r);

    vcoFreqSlider->setBounds(0, newTitleHeight, getWidth(), newTitleHeight);
    vcoDepthSlider->setBounds(0, 2 * newTitleHeight, getWidth(), newTitleHeight);
    // g.drawImageWithin(*background, 0, newTitleHeight, getWidth(), getHeight() - newTitleHeight, RectanglePlacement::stretchToFit, false);
}

void LeslieMaxwellEditor::resized()
{
    lastCenter = {static_cast<int>(lastCenterRelative.x * getWidth()), static_cast<int>(lastCenterRelative.y * getHeight())};
}

// MOUSE CALLBACKS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// void GainAudioProcessorEditor::mouseDoubleClick(const MouseEvent &event)
// {
//     if (event.mods.isRightButtonDown())
//     {
//         const auto w = static_cast<float>(getWidth());
//         const auto h = static_cast<float>(getHeight());
//
//         // Reset size and position
//         sizeMultiplier     = INITIAL_MULTIPLIER;
//         lastCenter         = event.getMouseDownPosition();
//         lastCenterRelative = {event.getMouseDownPosition().x / w, event.getMouseDownPosition().y / h};
//         offset             = {0, 0};
//
//         // repaintMouseChanges();
//     }
// }

// void GainAudioProcessorEditor::mouseDrag(const MouseEvent &event)
// {
//     if (event.mods.isRightButtonDown())
//     {
//         offset = event.getOffsetFromDragStart();
//         DBG("Drag offset: " << offset.x << ", " << offset.y);
//         // repaintMouseChanges();
//     }
// }

// void GainAudioProcessorEditor::mouseUp(const MouseEvent &event)
// {
//     if (event.mods.isRightButtonDown())
//     {
//         const auto w = static_cast<float>(getWidth());
//         const auto h = static_cast<float>(getHeight());
//         lastCenter += offset;
//         lastCenterRelative = {lastCenter.x / w, lastCenter.y / h};
//         offset             = {0, 0};
//         DBG("lastCenter: " << lastCenter.x << ", " << lastCenter.y);
//         DBG("lastCenterRelative: " << lastCenterRelative.x << ", " << lastCenterRelative.y);
//         // repaintMouseChanges();
//     }
// }

// void GainAudioProcessorEditor::mouseWheelMove(const MouseEvent &, const MouseWheelDetails &mouse_wheel_details)
// {
//     sizeMultiplier += mouse_wheel_details.deltaY * 0.1f;
//     sizeMultiplier = jlimit(0.0f, 1000.0f, sizeMultiplier);
//     DBG("sizeMultiplier: " << sizeMultiplier);
//     // repaintMouseChanges();
// }

// CUSTOM FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
