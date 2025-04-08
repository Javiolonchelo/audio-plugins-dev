//
// Created by javiolonchelo on 4/8/2025.
//

#include "CocoKnob.h"
void CocoKnob::paint(Graphics &g)
{
    Slider::paint(g);
}
void CocoKnob::mouseWheelMove(const MouseEvent &mouse_event, const MouseWheelDetails &mouse_wheel_details)
{
    sizeMultiplier += mouse_wheel_details.deltaY * 0.1f;
    repaint();
}