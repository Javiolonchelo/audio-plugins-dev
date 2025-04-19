// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "CocoKnob.h"

CocoKnob::CocoKnob() : Slider(Slider::RotaryVerticalDrag, Slider::NoTextBox)
{
    setDoubleClickReturnValue(true, 0.0);
    setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    setRotaryParameters(MathConstants<float>::pi * 4.0f / 3.0f, MathConstants<float>::pi * 8.0f / 3.0f, true);
}
void CocoKnob::mouseDoubleClick(const MouseEvent& event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p = getParentComponent();
        p->mouseDoubleClick(event.getEventRelativeTo(p));
    }
    else { Slider::mouseDoubleClick(event); }
}

void CocoKnob::mouseDrag(const MouseEvent& event)
{
    if (event.mods.isRightButtonDown())
    {
        const auto p = getParentComponent();
        p->mouseDrag(event.getEventRelativeTo(p));
    }
    else { Slider::mouseDrag(event); }
}

void CocoKnob::mouseDown(const MouseEvent& event) { Slider::mouseDown(event); }

void CocoKnob::mouseUp(const MouseEvent& event)
{
    const auto p = getParentComponent();
    p->mouseUp(event.getEventRelativeTo(p));
    Slider::mouseUp(event);
}

void CocoKnob::mouseWheelMove(const MouseEvent& mouse_event, const MouseWheelDetails& mouse_wheel_details)
{
    const auto p = getParentComponent();
    p->mouseWheelMove(mouse_event.getEventRelativeTo(p), mouse_wheel_details);
}
