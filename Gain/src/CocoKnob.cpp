//
// Created by javiolonchelo on 4/8/2025.
//

#include "CocoKnob.h"

// void CocoKnob::paint(Graphics& g)
// {
//     // dummy
//     const auto       sizeMultiplier = 0.3f;
//     const Point<int> lastCenter     = {200, 200};
//     const Point<int> offset         = {0, 0};
//
//     const auto newWidth  = jmax(sizeMultiplier * getParentComponent()->getWidth(), 0.0f);
//     const auto newHeight = jmax(sizeMultiplier * getParentComponent()->getHeight(), 0.0f);
//     jassert(newWidth >= 0 && newHeight >= 0);
//
//     const auto x = lastCenter.x + offset.x - newWidth / 2;
//     const auto y = lastCenter.y + offset.y - newHeight / 2;
//
//     setBounds(x, y, newWidth, newHeight);
//
//     Slider::paint(g);
// }

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
