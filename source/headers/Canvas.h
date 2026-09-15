#pragma once
#include "Colors.h"

class Canvas : public juce::Component {
public:
    explicit Canvas(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.fillAll(colors.getColor(ThemeColors::canvasBG));
    };

private:
    Colors &colors;
};
