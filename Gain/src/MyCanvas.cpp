//
// Created by javiolonchelo on 24/04/2025.
//

#include "MyCanvas.h"
MyCanvas::MyCanvas()
{
    background = std::make_unique<Image>(ImageCache::getFromMemory(BinaryData::background_jpg, BinaryData::background_jpgSize));
    jassert(background != nullptr && background->isValid());
}
void MyCanvas::paint(Graphics &g)
{
    // Background
    g.drawImageWithin(*background, 0, TITLE_HEIGHT, getWidth(), getHeight() - TITLE_HEIGHT, RectanglePlacement::stretchToFit, false);
}

void MyCanvas::mouseDoubleClick(const MouseEvent &event) { getParentComponent()->mouseDoubleClick(event); }
void MyCanvas::mouseDrag(const MouseEvent &event) { getParentComponent()->mouseDrag(event); }
void MyCanvas::mouseUp(const MouseEvent &event) { getParentComponent()->mouseUp(event); }
void MyCanvas::mouseDown(const MouseEvent &event) { getParentComponent()->mouseDown(event); }
void MyCanvas::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &mouse_wheel_details)
{
    getParentComponent()->mouseWheelMove(event, mouse_wheel_details);
}
