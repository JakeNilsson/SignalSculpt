#pragma once
#include "Colors.h"
#include "SiScLookAndFeel.h"

class Canvas : public juce::Component {
public:
    explicit Canvas(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.fillAll(colors.getColor(ThemeColors::canvasBG));
    };

private:
    Colors &colors;
};
