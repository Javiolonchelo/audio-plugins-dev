//
// Created by javiolonchelo on 23/04/2025.
//

#ifndef TITLEBOX_H
#define TITLEBOX_H

#include "common.h"
#include "CustomLookAndFeel.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"

using namespace juce;

class TitleBox : public Component, public SliderListener<Slider>
{
   public:
    explicit TitleBox(AudioProcessorValueTreeState& apvts);

    void sliderValueChanged(Slider*) override;
    void resized() override;
    void paint(Graphics& g) override;

   private:
    AudioProcessorValueTreeState& apvts;
    CustomLookAndFeel             customLookAndFeel;
    std::unique_ptr<Label>        title;
};

#endif  // TITLEBOX_H
