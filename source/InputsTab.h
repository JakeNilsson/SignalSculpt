#pragma once
#include "Colors.h"
#include "SiScLookAndFeel.h"

class InputsTab : public juce::Component{
public:
    explicit InputsTab(Colors &colorsRef) : colors(colorsRef){}

    void paint(juce::Graphics& g) override {
        g.setColour(colors.getColor(ThemeColors::input));
        g.fillPath(inputsTabPath);

        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.fillRect(inputsBG);

        g.saveState();

        g.addTransform (juce::AffineTransform::rotation (
            juce::MathConstants<float>::halfPi,
            inputsTab.getCentreX(),
            inputsTab.getCentreY()
        ));

        g.setFont(inputsFont);

        g.drawText ("INPUTS",
                    inputsTextBounds,
                    juce::Justification::right,
                    false);

        g.restoreState();
    };

    bool hitTest (int x, int y) override
    {
        return inputsTabPath.contains ((float) x, (float) y);
    }

    void setTabBounds() {
        const auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto tabHeight = height * 8/40;
        auto handleWidth = width * 5/8;

        auto tabHandleDiff = width - handleWidth;

        inputsTabPath.startNewSubPath(0, 0);
        inputsTabPath.lineTo(width, 0);
        inputsTabPath.lineTo(width, tabHeight);
        inputsTabPath.lineTo(handleWidth, tabHeight + tabHandleDiff);
        inputsTabPath.lineTo(handleWidth, height);
        inputsTabPath.lineTo(0, height);
        inputsTabPath.closeSubPath();

        inputsBG = bounds;
        inputsBG.removeFromRight((3.f * width) / 7.f);

        inputsTab.setBounds(0, 0, width, tabHeight);
        inputsTab.removeFromLeft((4.f * width) / 7.f);

        inputsTextBounds.setBounds(
            0.0f,
            0.0f,
            inputsTab.getHeight(),
            inputsTab.getWidth()
        );

        inputsTextBounds.setCentre (inputsTab.getCentre());

        auto baseTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::interVar_ttf, BinaryData::interVar_ttfSize
        );

        juce::FontVariableSetting semiboldSetting[] {
            { juce::FontFeatureTag("wght"), 600.0f }
        };

        auto semiboldTypeface = baseTypeface->cloneWithVariableSettings(semiboldSetting);

        auto fontOptions = juce::FontOptions(semiboldTypeface).withPointHeight(48.0f);
        inputsFont = juce::Font(fontOptions);
    }

private:
    juce::Path inputsTabPath;

    juce::Rectangle<float> inputsBG;
    juce::Rectangle<float> inputsTab;
    juce::Rectangle<float> inputsTextBounds;

    juce::Font inputsFont;

    Colors &colors;
};