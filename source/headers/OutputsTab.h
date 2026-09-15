#pragma once
#include "Colors.h"

class OutputsTab : public juce::Component{
public:
    explicit OutputsTab(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setColour(colors.getColor(ThemeColors::output));
        g.fillPath(outputsTabPath);
    };

    void setPath() {
        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto tabHeight = height * 1/3;
        auto handleWidthRatio = 5.f/8.f;

        auto handleWidth = width * (1 - handleWidthRatio);
        auto tabHandleDiff = width - (width * handleWidthRatio);

        outputsTabPath.startNewSubPath(bounds.getTopRight());
        outputsTabPath.lineTo(handleWidth, 0);
        outputsTabPath.lineTo(handleWidth, height - tabHeight - tabHandleDiff);
        outputsTabPath.lineTo(0, height - tabHeight);
        outputsTabPath.lineTo(0, height);
        outputsTabPath.lineTo(width, height);
        outputsTabPath.closeSubPath();
    }

private:
    juce::Path outputsTabPath;
    Colors &colors;
};