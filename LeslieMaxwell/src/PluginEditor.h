// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "juce_opengl/juce_opengl.h"
#include "common.h"
// #include "CustomLookAndFeel.h"
#include "PluginProcessor.h"
#include "BinaryData.h"

class LeslieMaxwellEditor : public AudioProcessorEditor, private Timer, private OpenGLRenderer
{
   public:
    explicit LeslieMaxwellEditor(LeslieMaxwellProcessor &);
    ~LeslieMaxwellEditor() override;
    void paint(Graphics &) override;
    void resized() override;
    // void timerCallback() override;

    void timerCallback() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

   private:
    // Mouse callbacks
    // void mouseDoubleClick(const MouseEvent &) override;
    // void mouseDrag(const MouseEvent &) override;
    // void mouseUp(const MouseEvent &) override;
    // void mouseWheelMove(const MouseEvent &, const MouseWheelDetails &) override;

    float previousFrameIndex = 0.0f;

    // Member variables
    LeslieMaxwellProcessor &audioProcessor;
    std::unique_ptr<Slider> vcoFreqSlider;
    std::unique_ptr<Slider> vcoDepthSlider;
    // CustomLookAndFeel           customLookAndFeel;
    // std::unique_ptr<Image>      background;
    // std::unique_ptr<TextButton> bypassButton;

    // Frames
    const char* const* names = BinaryData::namedResourceList;

    OpenGLContext openGLContext;
    std::vector<std::unique_ptr<OpenGLTexture>> frames;

    float        sizeMultiplier     = INITIAL_MULTIPLIER;
    Point<int>   offset             = {0, 0};
    Point<int>   lastCenter         = {STARTUP_CENTER, STARTUP_CENTER};
    Point<float> lastCenterRelative = {0.5f, 0.5f};

    // APVTS
    // std::unique_ptr<AudioProcessorValueTreeState>                  &apvts;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> vcoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> vcoDepthAttachment;
    // std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeslieMaxwellEditor)
};
