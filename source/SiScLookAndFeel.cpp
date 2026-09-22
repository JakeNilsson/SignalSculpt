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
    auto fill    = slider.findColour (juce::Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();

    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 3;
    auto arcRadius = radius - lineW * 0.5f;

    //if (slider.isEnabled())
    //{
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
        g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::square));
    //}

    auto notchBGOvershoot = 4.0f; // tweak this to taste — how far past the arc it pokes
    auto notchOvershoot = 1.5f; // tweak this to taste — how far past the arc it pokes
    auto notchBGRadius = arcRadius + notchBGOvershoot;
    auto notchRadius = arcRadius + notchOvershoot;

    juce::Point<float> notchBGEnd (bounds.getCentreX() + notchBGRadius * std::cos (toAngle - juce::MathConstants<float>::halfPi),
                                  bounds.getCentreY() + notchBGRadius * std::sin (toAngle - juce::MathConstants<float>::halfPi));

    juce::Path notchBG;
    notchBG.startNewSubPath (bounds.getCentreX(), bounds.getCentreY());
    notchBG.lineTo (notchBGEnd);

    juce::Point<float> notchEnd (bounds.getCentreX() + notchRadius * std::cos (toAngle - juce::MathConstants<float>::halfPi),
                                  bounds.getCentreY() + notchRadius * std::sin (toAngle - juce::MathConstants<float>::halfPi));

    juce::Path notch;
    notch.startNewSubPath (bounds.getCentreX(), bounds.getCentreY());
    notch.lineTo (notchEnd);

    g.setColour (colors.getColor(ThemeColors::tabBG));
    g.strokePath(notchBG, juce::PathStrokeType(static_cast<float>(12)));

    g.setColour (fill);
    g.strokePath(notch, juce::PathStrokeType(static_cast<float>(2)));

    auto jackBounds = bounds.reduced(8);
    g.setColour (colors.getColor(ThemeColors::canvasBG));
    g.fillEllipse(jackBounds);
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