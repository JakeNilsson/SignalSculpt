#pragma once
#include "Colors.h"

class InputsTab : public juce::Component{
public:
    explicit InputsTab(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setColour(colors.getColor(ThemeColors::input));
        g.fillPath(inputsTabPath);
    };

    void setPath() {
        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto tabHeight = height * 1/3;
        auto handleWidth = width * 5/8;

        auto tabHandleDiff = width - handleWidth;

        inputsTabPath.startNewSubPath(0, 0);
        inputsTabPath.lineTo(width, 0);
        inputsTabPath.lineTo(width, tabHeight);
        inputsTabPath.lineTo(handleWidth, tabHeight + tabHandleDiff);
        inputsTabPath.lineTo(handleWidth, height);
        inputsTabPath.lineTo(0, height);
        inputsTabPath.closeSubPath();
    }

private:
    juce::Path inputsTabPath;
    Colors &colors;
};