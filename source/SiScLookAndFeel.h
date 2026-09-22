#pragma once
#include "BinaryData.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Colors.h"

class SiScLookAndFeel : public juce::LookAndFeel_V4 {
public:
    explicit SiScLookAndFeel (Colors &colorsRef) : colors(colorsRef) { };

    void drawComboBox(juce::Graphics &, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &) override;
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;

    juce::Font getComboBoxFont (juce::ComboBox& box) override;

private:
    Colors &colors;
};
