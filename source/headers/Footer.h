#pragma once
#include "Colors.h"

class Footer : public juce::Component {
public:
    explicit Footer(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.fillPath(footerPath);
    };

    void setPath() {
        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto tabWidth = width * 1/4;
        auto handleHeightRatio = 5.f/8.f;

        auto handleHeight = height * (1 - handleHeightRatio);
        auto tabHandleDiff = height - (height * handleHeightRatio);

        footerPath.startNewSubPath(0, height);
        footerPath.lineTo(0, handleHeight);
        footerPath.lineTo(width - tabWidth - tabHandleDiff, handleHeight);
        footerPath.lineTo(width - tabWidth, 0);
        footerPath.lineTo(width, 0);
        footerPath.lineTo(width, height);
        footerPath.closeSubPath();
    }

private:
    juce::Path footerPath;
    Colors &colors;
};