#pragma once
#include "Colors.h"

class Canvas : public juce::Component {
public:
    explicit Canvas (Colors& colorsRef) : colors (colorsRef) {}

    void paint (juce::Graphics& g) override {
        auto b = getLocalBounds().toFloat();

        g.setColour (colors.getColor (ThemeColors::canvasBG));
        g.fillRect (b);

        g.setColour (colors.getColor (ThemeColors::tabBG));
        g.fillEllipse (b.getCentreX() - 10.0f, b.getCentreY() - 10.0f, 20.0f, 20.0f);
    }

private:
    Colors& colors;
};