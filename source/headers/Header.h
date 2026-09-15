#pragma once
#include <cmath>

#include "Colors.h"

class Header : public juce::Component {
public:
    explicit Header(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.fillPath(headerPath);
    };

    void setPath() {
        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto tabWidth = width * 1/4;
        auto handleHeight = height * 5/8;

        auto tabHandleDiff = height - handleHeight;

        headerPath.startNewSubPath(0, 0);
        headerPath.lineTo(0, height);
        headerPath.lineTo(tabWidth, height);
        headerPath.lineTo(tabWidth + tabHandleDiff, handleHeight);
        headerPath.lineTo(width, handleHeight);
        headerPath.lineTo(width, 0);
        headerPath.closeSubPath();
    }

private:
    juce::Path headerPath;
    Colors &colors;
};