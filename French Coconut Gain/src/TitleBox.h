// dBob Studio 2026
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "common.h"
#include "CustomLookAndFeel.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"

using namespace juce;

class TitleBox : public Component, public AudioProcessorValueTreeState::Listener, private AsyncUpdater, private Timer
{
   public:
    explicit TitleBox(AudioProcessorValueTreeState&);
    ~TitleBox() override;

    void parameterChanged(const String& parameterID, float newValue) override;
    void resized() override;
    void paint(Graphics& g) override;

    void mouseEnter(const MouseEvent& event) override;
    void mouseExit(const MouseEvent& event) override;

   private:
    void handleAsyncUpdate() override;
    void timerCallback() override;
    void updateButtonTransform(ImageButton* button, float baseAngle, float shakeAngle);
    float easeInOutCubic(float t);

    AudioProcessorValueTreeState& apvts;
    CustomLookAndFeel             customLookAndFeel;
    std::unique_ptr<Label>        title;

    // Buttons
    std::unique_ptr<ImageButton> bypassButton;
    std::unique_ptr<ImageButton> frenchButton;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> frenchAttachment;

    // Images
    Image croissantImage;
    Image flagImage;

    // Labels for buttons
    std::unique_ptr<Label> bypassLabel;
    std::unique_ptr<Label> frenchLabel;

    // Cached values for async update
    std::atomic<float> pendingGainValue{0.0f};
    std::atomic<bool> pendingFrenchMode{false};

    // Animation state
    float bypassStartAngle{0.0f};
    float bypassTargetAngle{0.0f};
    float frenchStartAngle{0.0f};
    float frenchTargetAngle{0.0f};
    float bypassProgress{1.0f};
    float frenchProgress{1.0f};
    bool bypassState{false};
    bool frenchState{false};
    static constexpr float ANIMATION_DURATION_MS = 300.0f;
    static constexpr float ANIMATION_STEP_MS = 8.0f;

    // Shake animation state
    bool bypassHovering{false};
    bool frenchHovering{false};
    float shakePhase{0.0f};
    static constexpr float SHAKE_AMPLITUDE = 0.08f;
    static constexpr float SHAKE_SPEED = 4.0f;
};
