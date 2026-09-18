#pragma once
#include <cmath>
#include "Colors.h"
#include "SiScLookAndFeel.h"

class Header : public juce::Component {
public:
    explicit Header(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setGradientFill(juce::ColourGradient::vertical(juce::Colour{0x40000000},
                                                       colors.getColor(ThemeColors::invisCanvasBG),
                                                       headerShadow));
        g.fillRect(headerShadow);

        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.fillPath(headerPath);

        g.setColour(colors.getColor(ThemeColors::neutral));
        g.strokePath(headerDividerPath, juce::PathStrokeType(3));

        g.setFont(bulletPointFont);

        g.drawText(juce::String::charToString (0x2022),
                    headerTab,
                    juce::Justification::centred,
                    false);

        g.setFont(headerFont);

        g.setColour(colors.getColor(ThemeColors::input));
        g.drawText(" Signal",
                    headerTab,
                    juce::Justification::left,
                    false);

        g.setColour(colors.getColor(ThemeColors::output));
        g.drawText("Sculpt ",
                    headerTab,
                    juce::Justification::right,
                    false);
    };

    bool hitTest (int x, int y) override
    {
        return headerPath.contains ((float) x, (float) y);
    }

    void setTabBounds() {
        headerPath.clear();
        headerDividerPath.clear();

        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto headerHeight = height / 2.f;

        auto tabWidth = width * 1/4;
        auto handleHeightRatio = 5.f/8.f;

        auto handleHeight = headerHeight * handleHeightRatio;
        auto tabHandleDiff = headerHeight - handleHeight;

        headerPath.startNewSubPath(0, 0);
        headerPath.lineTo(0, headerHeight);
        headerDividerPath.startNewSubPath(0, headerHeight);
        headerPath.lineTo(tabWidth, headerHeight);
        headerDividerPath.lineTo(tabWidth, headerHeight);
        headerPath.lineTo(tabWidth + tabHandleDiff, handleHeight);
        headerDividerPath.lineTo(tabWidth + tabHandleDiff, handleHeight);
        headerPath.lineTo(width, handleHeight);
        headerDividerPath.lineTo(width, handleHeight);
        headerPath.lineTo(width, 0);
        headerPath.closeSubPath();

        headerShadow.setBounds(0, handleHeight, width, std::floor(height - handleHeight));

        headerTab.setBounds(0, 0, tabWidth, headerHeight);

        auto baseTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::interVar_ttf, BinaryData::interVar_ttfSize
        );

        juce::FontVariableSetting semiboldSetting[] {
            { juce::FontFeatureTag("wght"), 600.0f }
        };

        auto semiboldTypeface = baseTypeface->cloneWithVariableSettings(semiboldSetting);

        auto fontOptions = juce::FontOptions(semiboldTypeface).withPointHeight(64.0f);
        auto bulletFontOptions = juce::FontOptions(semiboldTypeface).withPointHeight(48.0f);
        headerFont = juce::Font(fontOptions);
        bulletPointFont = juce::Font(bulletFontOptions);
    }

private:
    juce::Path headerPath;
    juce::Path headerDividerPath;

    juce::Rectangle<float> headerShadow;
    juce::Rectangle<float> headerTab;

    juce::Font headerFont;
    juce::Font bulletPointFont;

    Colors &colors;
};