//
// Created by javiolonchelo on 4/8/2025.
//

#include "CocoKnob.h"

void CocoKnob::mouseDoubleClick(const MouseEvent &mouse_event) {
    if (mouse_event.mods.isRightButtonDown()) {
        lastCenter = mouse_event.getMouseDownPosition();
        offset     = {0, 0};
        DBG("lastCenter: " << lastCenter.x << ", " << lastCenter.y);
    } else {
        DBG("Slider value: " << getValue());
        Slider::mouseDoubleClick(mouse_event);
    }
}
void CocoKnob::mouseDrag(const MouseEvent &event) {
    if (event.mods.isRightButtonDown()) {
        offset = event.getOffsetFromDragStart();
        DBG("Drag offset: " << offset.x << ", " << offset.y);
        repaint();
    } else {  // Behave like a normal slider
        DBG("Slider value: " << getValue());
        Slider::mouseDrag(event);
    }
}

void CocoKnob::mouseUp(const MouseEvent &event) {
    if (event.mods.isRightButtonDown()) {
        lastCenter += offset;
        offset = {0, 0};
        DBG("lastCenter: " << lastCenter.x << ", " << lastCenter.y);
        Slider::mouseUp(event);
    }
}

void CocoKnob::mouseWheelMove(const MouseEvent &, const MouseWheelDetails &mouse_wheel_details) {
    sizeMultiplier += mouse_wheel_details.deltaY * 0.1f;
    repaint();
}
