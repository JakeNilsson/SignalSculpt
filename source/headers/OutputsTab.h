#pragma once
#include "Colors.h"
#include "SiScLookAndFeel.h"

class OutputsTab : public juce::Component{
public:
    explicit OutputsTab(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setColour(colors.getColor(ThemeColors::output));
        g.fillPath(outputsTabPath);

        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.fillRect(outputsTab);
    };

    void setTabBounds() {
        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto tabHeight = height * 1/4;
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

        outputsTab = bounds;
        outputsTab.removeFromLeft((3.f * width) / 7.f);
    }

private:
    juce::Label outputsLabel {"outputs", "OUTPUTS"};

    juce::Path outputsTabPath;
    juce::Rectangle<float> outputsTab;
    Colors &colors;
};