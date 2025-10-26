// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"

#include <PluginProcessor.h>
#include <juce_opengl/juce_opengl.h>

// void LeslieMaxwellEditor::timerCallback()
// {
//
//     repaint();
//
// }
void LeslieMaxwellEditor::newOpenGLContextCreated()
{
    for (int i = 0; names[i] != nullptr; ++i)
    {
        String name = names[i];

        // Load only those matching our frame pattern
        if (name.startsWith("maxwell") && name.endsWith("png"))
        {
            int dataSize = 0;
            const void* dataPtr = BinaryData::getNamedResource(name.toRawUTF8(), dataSize);

            jassert(dataPtr != nullptr);
            Image         img = ImageFileFormat::loadFrom(dataPtr, dataSize);
            auto tex = std::make_unique<OpenGLTexture>();
            tex->loadImage(img);
            frames.push_back(std::move(tex));
        }
    }
}

constexpr int   TOTAL_FRAMES = 201;
constexpr float RATE         = 60.0f;

void LeslieMaxwellEditor::renderOpenGL()
{
    const auto vcoFreqValue = audioProcessor.apvts->getRawParameterValue(P_VCO_DEPTH_ID)->load();
    previousFrameIndex += vcoFreqValue * (TOTAL_FRAMES / RATE);
    if (previousFrameIndex >= TOTAL_FRAMES)
        previousFrameIndex -= TOTAL_FRAMES;
    const auto currentFrame = static_cast<int>(previousFrameIndex);

    gl::glViewport(0, 0, getWidth(), getHeight());
    gl::glMatrixMode(gl::GL_PROJECTION);
    gl::glLoadIdentity();
    gl::glOrtho(0.0, getWidth(), getHeight(), 0.0, -1.0, 1.0);
    gl::glMatrixMode(gl::GL_MODELVIEW);
    gl::glLoadIdentity();

    OpenGLHelpers::clear(Colours::black);
    gl::glDisable(gl::GL_LIGHTING);
    gl::glColor3f(1, 1, 1);
    gl::glEnable(gl::GL_TEXTURE_2D); // make sure 2D texturing is enabled

    frames[currentFrame]->bind();
    const auto bounds = getLocalBounds().toFloat();

    gl::glBegin(gl::GL_QUADS);
    gl::glTexCoord2f(0.0f, 0.0f); gl::glVertex2f(bounds.getX(), bounds.getBottom());
    gl::glTexCoord2f(1.0f, 0.0f); gl::glVertex2f(bounds.getRight(), bounds.getBottom());
    gl::glTexCoord2f(1.0f, 1.0f); gl::glVertex2f(bounds.getRight(), bounds.getY());
    gl::glTexCoord2f(0.0f, 1.0f); gl::glVertex2f(bounds.getX(), bounds.getY());
    gl::glEnd();

    frames[currentFrame]->unbind();
}

void LeslieMaxwellEditor::openGLContextClosing()
{
    frames.clear();
}

LeslieMaxwellEditor::LeslieMaxwellEditor(LeslieMaxwellProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);
    // LookAndFeel_V4::setDefaultLookAndFeel(&customLookAndFeel);

    // Layout management
    setResizable(true, false);
    // setResizeLimits(MIN_SIZE, MIN_SIZE, MAX_SIZE, MAX_SIZE);
    setSize(1440, 810);
    // getConstrainer()->setFixedAspectRatio(static_cast<float>(STARTUP_SIZE) / (STARTUP_SIZE + TITLE_HEIGHT));

    // Slider settings
    vcoFreqSlider  = std::make_unique<Slider>();
    vcoDepthSlider = std::make_unique<Slider>();
    addAndMakeVisible(*vcoFreqSlider);
    addAndMakeVisible(*vcoDepthSlider);

    // Attachments
    vcoFreqAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_FREQ_ID, *vcoFreqSlider);
    vcoDepthAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_DEPTH_ID, *vcoDepthSlider);

    startTimerHz(RATE);

    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(false);
    openGLContext.setComponentPaintingEnabled(true);

    resized();
}

LeslieMaxwellEditor::~LeslieMaxwellEditor()
{
    LookAndFeel_V4::setDefaultLookAndFeel(nullptr);
    openGLContext.detach();
    // knob->setLookAndFeel(nullptr);
}

void LeslieMaxwellEditor::paint(Graphics &)
{
    // Calculate title height using float math and convert to int to avoid narrowing warnings
    // g.fillAll(Colours::black);
    // openGLContext.triggerRepaint();

    // Note: layout is handled in resized(); keep paint minimal.
    // g.drawImageWithin(*background, 0, newTitleHeight, getWidth(), getHeight() - newTitleHeight, RectanglePlacement::stretchToFit, false);
}

void LeslieMaxwellEditor::resized()
{
    const auto height = 100;
    // Layout sliders at the top, then the OpenGL demo occupies the remaining area.
    if (vcoFreqSlider)  vcoFreqSlider->setBounds(0, 0, getWidth(), height);
    if (vcoDepthSlider) vcoDepthSlider->setBounds(0, height, getWidth(), height);
}
void LeslieMaxwellEditor::timerCallback()
{
    openGLContext.triggerRepaint();
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
