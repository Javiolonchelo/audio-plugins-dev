// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "MyCanvas.h"

#include "BinaryData.h"

MyCanvas::MyCanvas()
{
    versionLabel = std::make_unique<Label>();
    versionLabel->setText("v" + String(JucePlugin_VersionString), dontSendNotification);
    versionLabel->setJustificationType(Justification::bottomRight);
    versionLabel->setFont(customLookAndFeel.getCustomFont(10.0f));
    versionLabel->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(*versionLabel);
}

void MyCanvas::paint(Graphics &g)
{
    // Background
    g.drawImageWithin(*background, 0, TITLE_HEIGHT, getWidth(), getHeight() - TITLE_HEIGHT, RectanglePlacement::stretchToFit, false);
}

void MyCanvas::resized()
{
    const int labelWidth  = 60;
    const int labelHeight = 16;
    const int margin      = 8;
    versionLabel->setBounds(getWidth() - labelWidth - margin, getHeight() - labelHeight - margin, labelWidth, labelHeight);
}

void MyCanvas::changeBackground(bool mode)
{
    if (mode) {
        background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::roomforcrazycats_jpg, BinaryData::roomforcrazycats_jpgSize));
    } else {
        background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    }
    jassert(background != nullptr && background->isValid());
}

void MyCanvas::mouseDoubleClick(const MouseEvent &event) { getParentComponent()->mouseDoubleClick(event); }
void MyCanvas::mouseDrag(const MouseEvent &event) { getParentComponent()->mouseDrag(event); }
void MyCanvas::mouseUp(const MouseEvent &event) { getParentComponent()->mouseUp(event); }
void MyCanvas::mouseDown(const MouseEvent &event) { getParentComponent()->mouseDown(event); }
void MyCanvas::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &mouse_wheel_details)
{
    getParentComponent()->mouseWheelMove(event, mouse_wheel_details);
}
