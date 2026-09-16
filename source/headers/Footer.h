#pragma once
#include "Colors.h"
#include "SiScLookAndFeel.h"

class Footer : public juce::Component {
public:
    explicit Footer(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setGradientFill(juce::ColourGradient::vertical(colors.getColor(ThemeColors::invisCanvasBG),
                                                       juce::Colour{0x40000000},
                                                       footerShadow));
        g.fillRect(footerShadow);

        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.fillPath(footerPath);

        g.setColour(colors.getColor(ThemeColors::neutral));
        g.strokePath(footerDividerPath, juce::PathStrokeType(3));
    };

    void setTabBounds() {
        footerPath.clear();
        footerDividerPath.clear();

        auto bounds = getLocalBounds().toFloat();

        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto footerHeight = height / 2.f;

        auto tabWidth = width * 1/4;
        auto handleHeightRatio = 5.f/8.f;

        auto handleHeight = footerHeight + (footerHeight * (1 - handleHeightRatio));
        auto tabHandleDiff = footerHeight - (footerHeight * handleHeightRatio);

        footerPath.startNewSubPath(0, height);
        footerPath.lineTo(0, handleHeight);
        footerDividerPath.startNewSubPath(0, handleHeight);
        footerPath.lineTo(width - tabWidth - tabHandleDiff, handleHeight);
        footerDividerPath.lineTo(width - tabWidth - tabHandleDiff, handleHeight);
        footerPath.lineTo(width - tabWidth, footerHeight);
        footerDividerPath.lineTo(width - tabWidth, footerHeight);
        footerPath.lineTo(width, footerHeight);
        footerDividerPath.lineTo(width, footerHeight);
        footerPath.lineTo(width, height);
        footerPath.closeSubPath();

        footerShadow.setBounds(0, 0, width, handleHeight);
    }

private:
    juce::Path footerPath;
    juce::Path footerDividerPath;
    juce::Rectangle<float> footerShadow;
    Colors &colors;
};