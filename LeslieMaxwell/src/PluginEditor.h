// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "juce_opengl/juce_opengl.h"
#include "common.h"
// #include "CustomLookAndFeel.h"
#include "PluginProcessor.h"
#include "BinaryData.h"

class LeslieMaxwellEditor : public AudioProcessorEditor, public Timer
{
   public:
    explicit LeslieMaxwellEditor(LeslieMaxwellProcessor &);
    ~LeslieMaxwellEditor() override;
    void paint(Graphics &) override;
    void resized() override;

    void timerCallback() override;

   private:

    float previousFrameIndex = 0.0f;

    LeslieMaxwellProcessor &audioProcessor;
    std::unique_ptr<Slider> vcoFreqSlider;
    std::unique_ptr<Slider> vcoDepthSlider;
    std::unique_ptr<Slider> modSlider;

    // Frames
    const char* const* names = BinaryData::namedResourceList;
    std::vector<Image> frames;

    float        sizeMultiplier     = INITIAL_MULTIPLIER;
    Point<int>   offset             = {0, 0};
    Point<int>   lastCenter         = {STARTUP_CENTER, STARTUP_CENTER};
    Point<float> lastCenterRelative = {0.5f, 0.5f};

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> vcoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> vcoDepthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> modAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeslieMaxwellEditor)
};
