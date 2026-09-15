#pragma once
#include "Colors.h"

class Header : public juce::Component {
public:
    explicit Header(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.fillAll(colors.getColor(ThemeColors::tabBG));
    };

private:
    Colors &colors;
};