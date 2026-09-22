#include "SiScLookAndFeel.h"

void SiScLookAndFeel::drawComboBox(juce::Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &box) {
    {
        auto cornerSize = 5.f;
        juce::Rectangle<int> boxBounds (0, 0, width, height);

        g.setColour (colors.getColor(ThemeColors::canvasBG));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

        juce::Rectangle<int> arrowZone (width - 25, 0, 20, height); // height is 21 for now, not always
        juce::Path path;
        path.startNewSubPath ((float) arrowZone.getX() + 4.0f, (float) arrowZone.getY() + 6.0f);
        path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getBottom() - 6.0f);
        path.lineTo ((float) arrowZone.getRight() - 4.0f, (float) arrowZone.getY() + 6.0f);
        path.closeSubPath();

        g.setColour (colors.getColor(ThemeColors::tabBG));
        g.fillPath (path);
    }
}

void SiScLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider) {
    auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (juce::Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    auto thumbWidth = lineW * 2.0f;
    juce::Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - juce::MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - juce::MathConstants<float>::halfPi));

    g.setColour (slider.findColour (juce::Slider::thumbColourId));
    g.fillEllipse (juce::Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}

juce::Font SiScLookAndFeel::getComboBoxFont(juce::ComboBox &) {
    auto baseTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::kantumruyProVar_ttf, BinaryData::kantumruyProVar_ttfSize
        );

    juce::FontVariableSetting regularSetting[] {
        { juce::FontFeatureTag("wght"), 400.0f }
    };

    auto regularTypeface = baseTypeface->cloneWithVariableSettings(regularSetting);

    auto fontOptions = juce::FontOptions(regularTypeface).withPointHeight(16.0f);
    return {fontOptions};
}