#pragma once
#include "theme/Colors.h"
#include "theme/SiScLookAndFeel.h"

class InputsTab : public juce::Component{
public:
    explicit InputsTab(Colors &colorsRef, ConnectionOverlay& overlayRef) :
        colors(colorsRef),
        overlay(overlayRef),
        audIn(colorsRef, "AUD") {

        audIn.setID(std::to_underlying(StaticIDs::AudIn));
        audIn.setPort(0);
    }

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

        g.setColour(colors.getColor(ThemeColors::input));
        g.setFont(inputsFont.withHeight(26.f));
        g.drawText(audIn.getName(), audInLabelBounds.toNearestInt(), juce::Justification::centred);
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

        audInBounds.setBounds(inputsBG.getX(), inputsBG.getY() + 30, inputsBG.getWidth(), inputsBG.getWidth());
        audInBounds.reduce(8, 8);
        addAndMakeVisible(audIn);
        audIn.setBounds(audInBounds.toNearestInt());

        audInLabelBounds.setBounds(inputsBG.getX(), inputsBG.getY() + 10, inputsBG.getWidth(), 20);

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

        audIn.onDragStart = [this] (OutJack& outJack, const juce::MouseEvent&) {
            overlay.beginConnection (outJack, true);
        };

        audIn.onDrag = [this] (OutJack& outJack, const juce::Point<float> mousePos) {
            overlay.updateConnection (mousePos);
        };

        audIn.onDragEnd = [this] (OutJack& outJack, const juce::MouseEvent&) {
            overlay.cancelConnection ();
        };
    }

private:
    juce::Path inputsTabPath;

    juce::Rectangle<float> inputsBG;
    juce::Rectangle<float> inputsTab;
    juce::Rectangle<float> inputsTextBounds;

    juce::Font inputsFont;

    Colors &colors;

    ConnectionOverlay &overlay;

    OutJack audIn;
    juce::Rectangle<float> audInBounds;
    juce::Rectangle<float> audInLabelBounds;
};