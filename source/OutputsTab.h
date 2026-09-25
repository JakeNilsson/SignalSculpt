#pragma once
#include "theme/Colors.h"
#include "theme/SiScLookAndFeel.h"

class OutputsTab : public juce::Component{
public:
    explicit OutputsTab(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setColour(colors.getColor(ThemeColors::output));
        g.fillPath(outputsTabPath);

        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.fillRect(outputsBG);

        g.saveState();

        g.addTransform (juce::AffineTransform::rotation (
            -juce::MathConstants<float>::halfPi,
            outputsTab.getCentreX(),
            outputsTab.getCentreY()
        ));

        g.setFont(outputsFont);

        g.drawText ("OUTPUTS",
                    outputsTextBounds,
                    juce::Justification::right,
                    false);

        g.restoreState();
    };

    bool hitTest (int x, int y) override
    {
        return outputsTabPath.contains ((float) x, (float) y);
    }

    void setTabBounds() {
        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto tabHeight = height * 41/160; // yeah this number is crazy, it just looked the best imo don't judge me
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

        outputsBG = bounds;
        outputsBG.removeFromLeft((3.f * width) / 7.f);

        outputsTab.setBounds(0, height - tabHeight, width, tabHeight);
        outputsTab.removeFromRight((4.f * width) / 7.f);

        outputsTextBounds.setBounds(
            0.0f,
            0.0f,
            outputsTab.getHeight(),
            outputsTab.getWidth()
        );

        outputsTextBounds.setCentre (outputsTab.getCentre());

        auto baseTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::interVar_ttf, BinaryData::interVar_ttfSize
        );

        juce::FontVariableSetting semiboldSetting[] {
            { juce::FontFeatureTag("wght"), 600.0f }
        };

        auto semiboldTypeface = baseTypeface->cloneWithVariableSettings(semiboldSetting);

        auto fontOptions = juce::FontOptions(semiboldTypeface).withPointHeight(48.0f);
        outputsFont = juce::Font(fontOptions);
    }

private:
    juce::Path outputsTabPath;

    juce::Rectangle<float> outputsBG;
    juce::Rectangle<float> outputsTab;
    juce::Rectangle<float> outputsTextBounds;

    juce::Font outputsFont;

    Colors &colors;
};