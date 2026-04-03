// dBob Studio 2025
// Javier Rodrigo López
// javiolonchelo@gmail.com

#include "PluginEditor.h"

#include <PluginProcessor.h>

// Design dimensions — all layout is authored at this fixed resolution
static constexpr float DESIGN_WIDTH  = 960.0f;
static constexpr float DESIGN_TOP_H  = 960.0f * 379.0f / 1500.0f; // ≈ 242
static constexpr float DESIGN_HEIGHT = 540.0f + DESIGN_TOP_H;      // ≈ 782

LeslieMaxwellEditor::LeslieMaxwellEditor(LeslieMaxwellProcessor &p) : AudioProcessorEditor(&p), audioProcessor(p)
{
    // General settings and UI
    MouseEvent::setDoubleClickTimeout(DOUBLE_CLICK_TIMEOUT);

    // Layout management
    setResizable(true, false);
    setSize(static_cast<int>(DESIGN_WIDTH), static_cast<int>(DESIGN_HEIGHT));
    getConstrainer()->setFixedAspectRatio(DESIGN_WIDTH / DESIGN_HEIGHT);

    // Content container — children live here, scaled by a single AffineTransform
    addAndMakeVisible(content);

    // Slider settings
    vcoFreqSlider  = std::make_unique<SyncableSlider>();
    vcoFreqSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    vcoFreqSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    vcoFreqSlider->setName("SPEED");
    vcoDepthSlider = std::make_unique<SyncableSlider>();
    vcoDepthSlider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    vcoDepthSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    vcoDepthSlider->setName("DEPTH");
    content.addAndMakeVisible(*vcoFreqSlider);
    content.addAndMakeVisible(*vcoDepthSlider);

    // Attachments
    vcoFreqAttachment  = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_FREQ_ID, *vcoFreqSlider);
    vcoDepthAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(*p.apvts, P_VCO_DEPTH_ID, *vcoDepthSlider);

    // Mode selector
    modeComboBox = std::make_unique<ComboBox>();
    modeComboBox->addItem("Tremolo", 1);
    modeComboBox->addItem("Tremolo + Panning", 2);
    modeComboBox->addItem("Vibrato", 3);
    modeComboBox->addItem("Doppler", 4);
    content.addAndMakeVisible(*modeComboBox);
    modeAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(*p.apvts, P_MODE_ID, *modeComboBox);

    prevModeButton = std::make_unique<TextButton>("<");
    nextModeButton = std::make_unique<TextButton>(">");
    content.addAndMakeVisible(*prevModeButton);
    content.addAndMakeVisible(*nextModeButton);
    prevModeButton->onClick = [this] {
        const int current = modeComboBox->getSelectedItemIndex();
        modeComboBox->setSelectedItemIndex(current > 0 ? current - 1 : NumModes - 1);
    };
    nextModeButton->onClick = [this] {
        const int current = modeComboBox->getSelectedItemIndex();
        modeComboBox->setSelectedItemIndex(current < NumModes - 1 ? current + 1 : 0);
    };

    // Direction toggle (default off = -1, toggled on = 1)
    directionButton = std::make_unique<ToggleButton>();
    content.addAndMakeVisible(*directionButton);
    directionAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(*p.apvts, P_DIRECTION_ID, *directionButton);

    // Sync toggle
    syncButton = std::make_unique<ToggleButton>();
    content.addAndMakeVisible(*syncButton);
    syncAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(*p.apvts, P_SYNC_ID, *syncButton);

    // Cursors
    vcoFreqSlider->setMouseCursor(MouseCursor::DraggingHandCursor);
    vcoDepthSlider->setMouseCursor(MouseCursor::DraggingHandCursor);
    directionButton->setMouseCursor(MouseCursor::PointingHandCursor);
    syncButton->setMouseCursor(MouseCursor::PointingHandCursor);
    prevModeButton->setMouseCursor(MouseCursor::PointingHandCursor);
    nextModeButton->setMouseCursor(MouseCursor::PointingHandCursor);
    modeComboBox->setMouseCursor(MouseCursor::PointingHandCursor);

    // Custom look and feel (applies to all children via content)
    customLookAndFeel = std::make_unique<CustomLookAndFeel>();
    content.setLookAndFeel(customLookAndFeel.get());

    // Set child bounds once at fixed design coordinates
    {
        constexpr int knobSize = 170;
        constexpr int arrowW   = 32;
        constexpr int comboH   = 28;
        constexpr int comboW   = 160;
        const int topH         = static_cast<int>(DESIGN_TOP_H);
        const int knobY        = (topH - knobSize) / 2 + 5;
        constexpr int leftKnobX  = 45;
        const int rightKnobX     = static_cast<int>(DESIGN_WIDTH) - 45 - knobSize;

        vcoFreqSlider->setBounds(leftKnobX, knobY, knobSize, knobSize);
        vcoDepthSlider->setBounds(rightKnobX, knobY, knobSize, knobSize);

        // Mode selector: combo box between arrows, arrows at fixed x positions
        constexpr int leftArrowX  = 460;
        constexpr int rightArrowX = 683;
        constexpr int modeY       = 165;
        const int comboX          = leftArrowX + arrowW;
        const int comboFitW       = rightArrowX - comboX;

        prevModeButton->setBounds(leftArrowX, modeY, arrowW, comboH);
        modeComboBox->setBounds(comboX, modeY, comboFitW, comboH);
        nextModeButton->setBounds(rightArrowX, modeY, arrowW, comboH);

        // Reverse button: centre (445, 280) in 1500×379 source, ~110×54
        constexpr int revW = static_cast<int>(110.0f / 1500.0f * 960.0f);  // ≈ 70
        constexpr int revH = static_cast<int>(54.0f / 379.0f * 242.0f);    // ≈ 34
        const int revX = static_cast<int>(445.0f / 1500.0f * DESIGN_WIDTH) - revW / 2;
        const int revY = static_cast<int>(280.0f / 379.0f * DESIGN_TOP_H)  - revH / 2;
        directionButton->setBounds(revX, revY, revW, revH);

        // Sync button: centre (594, 280) in 1500×379 source, ~110×54
        const int syncX = static_cast<int>(594.0f / 1500.0f * DESIGN_WIDTH) - revW / 2;
        const int syncY = static_cast<int>(280.0f / 379.0f * DESIGN_TOP_H)  - revH / 2;
        syncButton->setBounds(syncX, syncY, revW, revH);
    }

    // Filmstrip interaction area — drag/scroll for phase, hover for easter egg
    {
        const int topH = static_cast<int>(DESIGN_TOP_H);
        const int filmH = static_cast<int>(DESIGN_HEIGHT) - topH;
        content.addAndMakeVisible(filmstripArea);
        filmstripArea.setBounds(0, topH, static_cast<int>(DESIGN_WIDTH), filmH);
        filmstripArea.setAlwaysOnTop(true);

        filmstripArea.onPhaseDelta = [this](float deltaRad) {
            auto* param = audioProcessor.apvts->getParameter(P_PHASE_ID);
            const float twoPi = MathConstants<float>::twoPi;
            float current = param->convertFrom0to1(param->getValue());
            current += deltaRad;
            current = std::fmod(current, twoPi);
            if (current < 0.0f) current += twoPi;
            param->setValueNotifyingHost(param->convertTo0to1(current));
        };
    }

    // Load all assets synchronously so nothing paints before they're ready
    {
        const char* stripNames[NUM_STRIPS] = { "maxwell_strip_0_jpg", "maxwell_strip_1_jpg" };
        for (int i = 0; i < NUM_STRIPS; ++i)
        {
            int dataSize = 0;
            const void* dataPtr = BinaryData::getNamedResource(stripNames[i], dataSize);
            jassert(dataPtr != nullptr);
            strips[i] = ImageFileFormat::loadFrom(dataPtr, dataSize);
        }
        frameHeight = strips[0].getHeight() / FRAMES_PER_STRIP[0];

        int topSize = 0;
        const void* topPtr = BinaryData::getNamedResource("top_png", topSize);
        jassert(topPtr != nullptr);
        topImage = ImageFileFormat::loadFrom(topPtr, topSize);

        auto loadBtn = [](const char* name) {
            int sz = 0;
            const void* ptr = BinaryData::getNamedResource(name, sz);
            return (ptr != nullptr) ? ImageFileFormat::loadFrom(ptr, sz) : Image();
        };
        revOnImg   = loadBtn("rev_on_png");
        revOffImg  = loadBtn("rev_off_png");
        revHoldImg = loadBtn("rev_hold_png");
        syncOnImg   = loadBtn("sync_on_png");
        syncOffImg  = loadBtn("sync_off_png");
        syncHoldImg = loadBtn("sync_hold_png");

        stripsLoaded.store(true);
    }

    resized();
    startTimerHz(60);

#if JUCE_DEBUG
    inspector = std::make_unique<melatonin::Inspector>(*this, false);
#endif
}

LeslieMaxwellEditor::~LeslieMaxwellEditor()
{
    content.setLookAndFeel(nullptr);
}

void LeslieMaxwellEditor::paint(Graphics &g)
{
    g.fillAll(Colours::black);

    if (!stripsLoaded.load())
        return;

    // Single scale factor from design resolution to actual window size
    const float scale = static_cast<float>(getWidth()) / DESIGN_WIDTH;
    g.addTransform(AffineTransform::scale(scale));

    // Top banner at design coordinates
    const int topH = static_cast<int>(DESIGN_TOP_H);
    const int dw   = static_cast<int>(DESIGN_WIDTH);
    g.drawImage(topImage, 0, 0, dw, topH,
                0, 0, topImage.getWidth(), topImage.getHeight());

    // Direction button overlay (same size as top banner, drawn on top)
    {
        const auto& revImg = directionButton->isMouseButtonDown() ? revHoldImg
                           : directionButton->getToggleState()     ? revOnImg
                                                                    : revOffImg;
        if (revImg.isValid())
            g.drawImage(revImg, 0, 0, dw, topH,
                        0, 0, revImg.getWidth(), revImg.getHeight());
    }

    // Sync button overlay (same size as top banner, drawn on top)
    {
        const auto& syncImg = syncButton->isMouseButtonDown() ? syncHoldImg
                            : syncButton->getToggleState()     ? syncOnImg
                                                                : syncOffImg;
        if (syncImg.isValid())
            g.drawImage(syncImg, 0, 0, dw, topH,
                        0, 0, syncImg.getWidth(), syncImg.getHeight());
    }

    // Filmstrip frame below the top banner
    int frameIndex = static_cast<int>(previousFrameIndex);

    int stripIndex = 0;
    int localFrame = frameIndex;
    while (stripIndex < NUM_STRIPS - 1 && localFrame >= FRAMES_PER_STRIP[stripIndex])
    {
        localFrame -= FRAMES_PER_STRIP[stripIndex];
        ++stripIndex;
    }

    auto frameRegion = strips[stripIndex].getClippedImage({ 0, localFrame * frameHeight, strips[stripIndex].getWidth(), frameHeight });

    const int filmY = topH;
    const int filmH = static_cast<int>(DESIGN_HEIGHT) - topH;

    // Use the parameter's 0-1 normalised position (accounts for skew) so FOV starts from min knob value
    auto* depthParam = audioProcessor.apvts->getParameter(P_VCO_DEPTH_ID);
    const float depthNorm = depthParam->getValue(); // 0–1 linear w.r.t. knob position

    if (depthNorm > 0.01f)
    {
        // Real barrel distortion (fisheye FOV) driven by depth
        const int srcW = frameRegion.getWidth();
        const int srcH = frameRegion.getHeight();

        if (fovBuffer.getWidth() != srcW || fovBuffer.getHeight() != srcH)
            fovBuffer = Image(frameRegion.getFormat(), srcW, srcH, false);

        const float k = std::pow(depthNorm, 1.85f) * 3.13f; // barrel distortion strength
        const float cx = srcW * 0.5f;
        const float cy = srcH * 0.5f;

        // Solve z + 2k·z³ = 1 (Newton) so corner pixels map to source edge — no black
        float z = 1.0f / (1.0f + k);
        for (int i = 0; i < 3; ++i)
        {
            const float f  = 2.0f * k * z * z * z + z - 1.0f;
            const float fp = 6.0f * k * z * z + 1.0f;
            z -= f / fp;
        }
        const float preZoom = z;

        Image::BitmapData src(frameRegion, Image::BitmapData::readOnly);
        Image::BitmapData dst(fovBuffer, Image::BitmapData::writeOnly);
        const int ps = src.pixelStride;

        for (int y = 0; y < srcH; ++y)
        {
            uint8_t* dstLine = dst.getLinePointer(y);

            for (int x = 0; x < srcW; ++x)
            {
                // Normalised coords pre-zoomed so distorted corners stay in bounds
                const float nx = (x - cx) / cx * preZoom;
                const float ny = (y - cy) / cy * preZoom;
                const float r2 = nx * nx + ny * ny;

                // Barrel distortion: centre magnified, edges compressed
                const float factor = 1.0f + k * r2;
                const float sx = nx * factor * cx + cx;
                const float sy = ny * factor * cy + cy;

                // Bilinear interpolation
                const int x0 = static_cast<int>(sx);
                const int y0 = static_cast<int>(sy);

                uint8_t* dp = dstLine + x * ps;

                if (x0 >= 0 && x0 < srcW - 1 && y0 >= 0 && y0 < srcH - 1)
                {
                    const float fx = sx - x0;
                    const float fy = sy - y0;
                    const float w00 = (1.0f - fx) * (1.0f - fy);
                    const float w10 = fx * (1.0f - fy);
                    const float w01 = (1.0f - fx) * fy;
                    const float w11 = fx * fy;

                    const uint8_t* p00 = src.getLinePointer(y0)     + x0 * ps;
                    const uint8_t* p10 = p00 + ps;
                    const uint8_t* p01 = src.getLinePointer(y0 + 1) + x0 * ps;
                    const uint8_t* p11 = p01 + ps;

                    for (int c = 0; c < ps; ++c)
                        dp[c] = static_cast<uint8_t>(p00[c] * w00 + p10[c] * w10 + p01[c] * w01 + p11[c] * w11);
                }
                else
                {
                    std::memset(dp, 0, static_cast<size_t>(ps));
                }
            }
        }

        g.drawImageWithin(fovBuffer, 0, filmY, dw, filmH, RectanglePlacement::centred);
    }
    else
    {
        g.drawImageWithin(frameRegion, 0, filmY, dw, filmH, RectanglePlacement::centred);
    }

    // Phase edit overlay text
    if (filmstripArea.phaseEditActive)
    {
        const bool showText = !filmstripArea.isBlinking() || filmstripArea.isBlinkVisible();
        if (showText)
        {
            const String phaseText = "SETTING INITIAL PHASE";
            g.setFont(customLookAndFeel->getCustomFont(18.0f));
            g.setColour(Colour(0xBBFFFFFF));
            g.drawText(phaseText, 12, filmY + 10, 300, 24, Justification::centredLeft, false);
        }
    }

    // Easter egg tooltip at cursor position
    if (filmstripArea.showTooltip)
    {
        const String text = "Rotate me, Father, for I have synthed.";
        g.setFont(customLookAndFeel->getCustomFont(20.0f));

        const int tw = static_cast<int>(g.getCurrentFont().getStringWidthFloat(text) + 24);
        const int th = static_cast<int>(20.0f + 16);
        // Convert cursor pos from filmstrip local coords to design coords
        auto cursorInEditor = filmstripArea.localPointToGlobal(filmstripArea.cursorPos);
        auto cursorInContent = content.getLocalPoint(nullptr, cursorInEditor);
        const int tx = cursorInContent.x + 12;
        const int ty = cursorInContent.y - th - 8;

        g.setColour(Colour(0xDD000000));
        g.fillRoundedRectangle(static_cast<float>(tx), static_cast<float>(ty),
                               static_cast<float>(tw), static_cast<float>(th), 6.0f);
        g.setColour(Colours::white);
        g.drawText(text, tx, ty, tw, th, Justification::centred, false);
    }
}

void LeslieMaxwellEditor::resized()
{
    // Scale the content container via a single AffineTransform — no per-widget arithmetic
    const float scale = static_cast<float>(getWidth()) / DESIGN_WIDTH;
    content.setBounds(0, 0, static_cast<int>(DESIGN_WIDTH), static_cast<int>(DESIGN_HEIGHT));
    content.setTransform(AffineTransform::scale(scale));
}

void LeslieMaxwellEditor::timerCallback()
{
    // Phase edit mode: exit after 1-second cooldown
    if (filmstripArea.phaseEditActive && filmstripArea.cooldownExpired())
    {
        filmstripArea.phaseEditActive = false;
        filmstripArea.phaseEditDoneTime = 0;
    }

    // Frame index: static preview during phase edit, animated otherwise
    static constexpr float phaseOffset = 183.0f / (NUM_FRAMES - 1) * MathConstants<float>::twoPi;
    const float userPhase = audioProcessor.apvts->getRawParameterValue(P_PHASE_ID)->load();

    if (filmstripArea.phaseEditActive)
    {
        // Show cat at t=0 position (only user phase + static offset, no VCO animation)
        const float staticPhase = std::fmod(phaseOffset + userPhase + MathConstants<float>::twoPi,
                                            MathConstants<float>::twoPi);
        previousFrameIndex = staticPhase * (NUM_FRAMES - 1) / MathConstants<float>::twoPi;
    }
    else
    {
        const float phase = std::fmod(audioProcessor.vcoPhase[0] + phaseOffset + userPhase + MathConstants<float>::twoPi,
                                      MathConstants<float>::twoPi);
        previousFrameIndex = phase * (NUM_FRAMES - 1) / MathConstants<float>::twoPi;
    }

    // Update LookAndFeel and slider sync state
    const bool syncOn = audioProcessor.apvts->getRawParameterValue(P_SYNC_ID)->load() > 0.5f;
    customLookAndFeel->syncMode = syncOn;
    vcoFreqSlider->syncSnapEnabled = syncOn;

    // Snap to nearest division when sync is toggled on
    if (syncOn && !prevSyncOn)
    {
        auto* param = audioProcessor.apvts->getParameter(P_VCO_FREQ_ID);
        const float normPos = param->getValue();
        const int targetDiv = jlimit(0, NUM_SYNC_DIVISIONS - 1,
                                     static_cast<int>(std::round(normPos * (NUM_SYNC_DIVISIONS - 1))));
        const float snappedNorm = static_cast<float>(targetDiv) / (NUM_SYNC_DIVISIONS - 1);
        if (std::abs(normPos - snappedNorm) > 0.001f)
            param->setValueNotifyingHost(snappedNorm);
    }
    prevSyncOn = syncOn;

    // Easter egg: show tooltip after 3s hover
    if (filmstripArea.hoverStart != 0 && Time::getMillisecondCounter() - filmstripArea.hoverStart >= 10000)
        filmstripArea.showTooltip = true;

    repaint();
}
