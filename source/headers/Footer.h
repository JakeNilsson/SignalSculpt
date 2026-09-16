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

        g.setFont(footerFont);

        g.drawText ("M A C R O S",
                    footerTab,
                    juce::Justification::centred,
                    false);
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

        footerTab.setBounds(width - tabWidth, footerHeight, tabWidth, footerHeight);

        auto baseTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::interVar_ttf, BinaryData::interVar_ttfSize
        );

        juce::FontVariableSetting semiboldSetting[] {
            { juce::FontFeatureTag("wght"), 800.0f }
        };

        auto semiboldTypeface = baseTypeface->cloneWithVariableSettings(semiboldSetting);

        auto fontOptions = juce::FontOptions(semiboldTypeface).withPointHeight(64.0f);
        footerFont = juce::Font(fontOptions);
    }

private:
    juce::Path footerPath;
    juce::Path footerDividerPath;

    juce::Rectangle<float> footerShadow;
    juce::Rectangle<float> footerTab;

    juce::Font footerFont;

    Colors &colors;
};