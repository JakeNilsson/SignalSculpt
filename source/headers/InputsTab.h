#pragma once
#include "Colors.h"

class InputsTab : public juce::Component{
public:
    explicit InputsTab(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.fillAll(colors.getColor(ThemeColors::input));
    };

private:
    Colors &colors;
};