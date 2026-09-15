#pragma once
#include "Colors.h"

class OutputsTab : public juce::Component{
public:
    explicit OutputsTab(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.fillAll(colors.getColor(ThemeColors::output));
    };

private:
    Colors &colors;
};