// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#pragma once

#include "common.h"

#if JUCE_DEBUG
 #include "melatonin_inspector/melatonin_inspector.h"
#endif
#include "CustomLookAndFeel.h"
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

    // Slider subclass that snaps to sync divisions
    struct SyncableSlider : public Slider
    {
        bool syncSnapEnabled = false;

        void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) override
        {
            if (!syncSnapEnabled)
            {
                auto scaled = wheel;
                scaled.deltaY *= 0.4f;
                Slider::mouseWheelMove(e, scaled);
                return;
            }

            const double normPos = valueToProportionOfLength(getValue());
            int cur = jlimit(0, NUM_SYNC_DIVISIONS - 1,
                             static_cast<int>(std::round(normPos * (NUM_SYNC_DIVISIONS - 1))));
            int next = cur;
            if (wheel.deltaY > 0.0f && cur < NUM_SYNC_DIVISIONS - 1)
                next = cur + 1;
            else if (wheel.deltaY < 0.0f && cur > 0)
                next = cur - 1;
            if (next != cur)
                setValue(proportionOfLengthToValue(
                    static_cast<double>(next) / (NUM_SYNC_DIVISIONS - 1)),
                    sendNotification);
        }

        double snapValue(double attemptedValue, DragMode) override
        {
            if (!syncSnapEnabled) return attemptedValue;

            const double normPos = valueToProportionOfLength(attemptedValue);
            const int targetDiv = jlimit(0, NUM_SYNC_DIVISIONS - 1,
                                         static_cast<int>(std::round(normPos * (NUM_SYNC_DIVISIONS - 1))));
            return proportionOfLengthToValue(
                static_cast<double>(targetDiv) / (NUM_SYNC_DIVISIONS - 1));
        }
    };

    float previousFrameIndex = 0.0f;
    bool  prevSyncOn = false;

    // Container for all child widgets — scaled via AffineTransform to avoid text wiggle
    Component content;

    LeslieMaxwellProcessor &audioProcessor;
    std::unique_ptr<SyncableSlider> vcoFreqSlider;
    std::unique_ptr<SyncableSlider> vcoDepthSlider;
    std::unique_ptr<CustomLookAndFeel> customLookAndFeel;

    // Mode selector
    std::unique_ptr<ComboBox> modeComboBox;
    std::unique_ptr<TextButton> prevModeButton;
    std::unique_ptr<TextButton> nextModeButton;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;

    // Direction toggle
    std::unique_ptr<ToggleButton> directionButton;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> directionAttachment;

    // Sync toggle
    std::unique_ptr<ToggleButton> syncButton;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> syncAttachment;

    // Top banner
    Image topImage;

    // Button overlays (same size as top banner, drawn on top)
    Image revOnImg, revOffImg, revHoldImg;
    Image syncOnImg, syncOffImg, syncHoldImg;

    // FOV barrel distortion buffer (reused across frames)
    Image fovBuffer;

    // Filmstrips (split across 2 JPEG files)
    Image strips[NUM_STRIPS];
    int frameHeight = 0;
    std::atomic<bool> stripsLoaded { false };

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> vcoFreqAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> vcoDepthAttachment;

    // Filmstrip interaction area — drag/scroll to change phase, hover centre for easter egg
    struct FilmstripArea : public Component, public Timer
    {
        std::function<void(float)> onPhaseDelta;

        // Drag + inertia
        static constexpr float SENSITIVITY  = 0.005f;
        static constexpr float FRICTION     = 0.92f;
        static constexpr float MIN_VELOCITY = 0.0005f;
        float velocity = 0.0f;
        int   lastDragX = 0;
        bool  dragging = false;

        // Phase edit mode: active during drag/inertia + 1s after
        bool   phaseEditActive = false;
        uint32 phaseEditDoneTime = 0; // ms timestamp when inertia stopped (0 = still moving)

        static constexpr int   BLINK_COUNT    = 3;
        static constexpr uint32 BLINK_ON_MS    = 150;
        static constexpr uint32 BLINK_OFF_MS   = 150;
        static constexpr uint32 BLINK_CYCLE_MS = BLINK_ON_MS + BLINK_OFF_MS;
        static constexpr uint32 BLINK_TOTAL_MS = BLINK_COUNT * BLINK_CYCLE_MS; // 900ms

        bool isBlinking() const { return phaseEditDoneTime != 0; }
        bool isBlinkVisible() const
        {
            const uint32 elapsed = Time::getMillisecondCounter() - phaseEditDoneTime;
            return (elapsed % BLINK_CYCLE_MS) < BLINK_ON_MS;
        }
        bool cooldownExpired() const
        {
            return phaseEditDoneTime != 0
                && Time::getMillisecondCounter() - phaseEditDoneTime >= BLINK_TOTAL_MS;
        }

        // Easter egg hover — 400px diameter circle at centre
        static constexpr float EGG_RADIUS = 200.0f;
        uint32 hoverStart = 0;
        bool   showTooltip = false;
        Point<int> cursorPos;

        void mouseDown(const MouseEvent& e) override
        {
            velocity = 0.0f;
            lastDragX = e.x;
            dragging = true;
            phaseEditActive = true;
            phaseEditDoneTime = 0;
            stopTimer();
        }

        void mouseDrag(const MouseEvent& e) override
        {
            const float dx = static_cast<float>(e.x - lastDragX);
            lastDragX = e.x;
            velocity = -dx * SENSITIVITY;
            if (onPhaseDelta) onPhaseDelta(velocity);
        }

        void mouseUp(const MouseEvent&) override
        {
            dragging = false;
            if (std::abs(velocity) > MIN_VELOCITY)
                startTimerHz(60);
            else
                phaseEditDoneTime = Time::getMillisecondCounter();
        }

        void mouseWheelMove(const MouseEvent&, const MouseWheelDetails& wheel) override
        {
            phaseEditActive = true;
            phaseEditDoneTime = 0;
            velocity += wheel.deltaY * SENSITIVITY * 40.0f;
            if (!isTimerRunning())
                startTimerHz(60);
        }

        void timerCallback() override
        {
            velocity *= FRICTION;
            if (std::abs(velocity) < MIN_VELOCITY)
            {
                velocity = 0.0f;
                stopTimer();
                phaseEditDoneTime = Time::getMillisecondCounter();
                return;
            }
            if (onPhaseDelta) onPhaseDelta(velocity);
        }

        // Easter egg: hover detection in central circle
        void mouseEnter(const MouseEvent&) override { updateHover(true); }
        void mouseExit(const MouseEvent&) override  { hoverStart = 0; showTooltip = false; }
        void mouseMove(const MouseEvent& e) override
        {
            cursorPos = e.getPosition();
            updateHover(isInEggCircle(e.x, e.y));
        }

        void paint(Graphics&) override {}
        MouseCursor getMouseCursor() override { return MouseCursor::NormalCursor; }

    private:
        bool isInEggCircle(int x, int y) const
        {
            const float cx = getWidth() * 0.5f, cy = getHeight() * 0.5f;
            return (x - cx) * (x - cx) + (y - cy) * (y - cy) <= EGG_RADIUS * EGG_RADIUS;
        }

        void updateHover(bool inCircle)
        {
            if (inCircle)
            {
                if (hoverStart == 0) hoverStart = Time::getMillisecondCounter();
            }
            else
            {
                hoverStart = 0;
                showTooltip = false;
            }
        }
    };

    FilmstripArea filmstripArea;

#if JUCE_DEBUG
    std::unique_ptr<melatonin::Inspector> inspector;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeslieMaxwellEditor)
};
