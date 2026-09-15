#pragma once
#include "Colors.h"

class Footer : public juce::Component {
public:
    explicit Footer(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.fillAll(colors.getColor(ThemeColors::tabBG));
    };

private:
    Colors &colors;
};
