#include "SiScLookAndFeel.h"
#include "SiScSlider.h"

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

    constexpr auto knobIndicDiff = 10;

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
    bounds.reduce(knobIndicDiff, knobIndicDiff);

    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 2.f;
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path valueArc;

    const auto rotaryParams = slider.getRotaryParameters();
    if (rotaryParams.stopAtEnd) {
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);
    } else {
        valueArc.addCentredArc (bounds.getCentreX(),
                            bounds.getCentreY(),
                            arcRadius,
                            arcRadius,
                            0.0f,
                            rotaryStartAngle,
                            rotaryEndAngle,
                            true);
    }

    g.setColour (fill);
    g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::square));

    auto notchBGOvershoot = 4.0f;
    auto notchOvershoot = 1.5f;
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
    g.strokePath(notchBG, juce::PathStrokeType(static_cast<float>(11)));

    g.setColour (fill);
    g.strokePath(notch, juce::PathStrokeType(static_cast<float>(1)));

    auto tabCircleBounds = bounds.reduced(3);
    g.setColour (colors.getColor(ThemeColors::tabBG));
    g.fillEllipse(tabCircleBounds);

    auto jackBounds = bounds.reduced(5);
    g.setColour (colors.getColor(ThemeColors::canvasBG));
    g.fillEllipse(jackBounds);

    if (auto* siScSlider = dynamic_cast<SiScSlider*> (&slider))
    {
        const auto snapValues= siScSlider->getSnapValues();
        const auto indicatorType = siScSlider->getIndicatorType();
        const auto latchedValue = siScSlider->getLatchedValue();

        bounds.expand(knobIndicDiff, knobIndicDiff);

        if (indicatorType == IndicatorType::circle) {
            constexpr auto circleWH = 4.f;
            auto indicatorRadius = arcRadius + knobIndicDiff - (circleWH / 2);
            float indicatorAngle;
            juce::Rectangle<float> indicatorBounds;
            juce::Point<float> topLeftBounds;

            for (const auto value : snapValues) {
                bool isLoopedPoint = false;

                if (!rotaryParams.stopAtEnd && latchedValue.has_value()) {
                    isLoopedPoint = (juce::approximatelyEqual(value, slider.getMinimum()) &&
                                     juce::approximatelyEqual(latchedValue.value(), slider.getMaximum()));
                }

                bool isDirectMatch = latchedValue.has_value() &&
                          juce::approximatelyEqual(value, latchedValue.value());

                if (isDirectMatch || isLoopedPoint) {
                    g.setColour(fill);
                } else {
                    g.setColour (colors.getColor(ThemeColors::canvasBG));
                }

                indicatorAngle = rotaryStartAngle + static_cast<float>(slider.valueToProportionOfLength(value)) * (rotaryEndAngle - rotaryStartAngle);

                topLeftBounds.setXY(bounds.getCentreX() + indicatorRadius * std::cos (indicatorAngle - juce::MathConstants<float>::halfPi) - (circleWH / 2),
                                    bounds.getCentreY() + indicatorRadius * std::sin (indicatorAngle - juce::MathConstants<float>::halfPi) - (circleWH / 2));

                indicatorBounds.setBounds(topLeftBounds.getX(), topLeftBounds.getY(), circleWH, circleWH);

                g.fillEllipse(indicatorBounds);
            }
        } else if (indicatorType == IndicatorType::line) {
            constexpr auto lineL = 5.f;
            constexpr auto lineW = 2.f;
            auto indicatorRadius = arcRadius + knobIndicDiff - lineL;
            auto outerRadius = indicatorRadius + lineL;

            float indicatorAngle;
            juce::Point<float> startPt;
            juce::Point<float> endPt;

            for (const auto value : snapValues) {
                bool isLoopedPoint = false;

                if (!rotaryParams.stopAtEnd && latchedValue.has_value()) {
                    isLoopedPoint = (juce::approximatelyEqual(value, slider.getMinimum()) &&
                                     juce::approximatelyEqual(latchedValue.value(), slider.getMaximum()));
                }

                bool isDirectMatch = latchedValue.has_value() &&
                          juce::approximatelyEqual(value, latchedValue.value());

                if (isDirectMatch || isLoopedPoint) {
                    g.setColour(fill);
                } else {
                    g.setColour (colors.getColor(ThemeColors::canvasBG));
                }

                indicatorAngle = rotaryStartAngle + static_cast<float>(slider.valueToProportionOfLength(value)) * (rotaryEndAngle - rotaryStartAngle);

                const auto cosA = std::cos (indicatorAngle - juce::MathConstants<float>::halfPi);
                const auto sinA = std::sin (indicatorAngle - juce::MathConstants<float>::halfPi);

                startPt.setXY (bounds.getCentreX() + indicatorRadius * cosA,
                                bounds.getCentreY() + indicatorRadius * sinA);

                endPt.setXY (bounds.getCentreX() + outerRadius * cosA,
                              bounds.getCentreY() + outerRadius * sinA);

                juce::Path indicatorLine;
                indicatorLine.startNewSubPath (startPt);
                indicatorLine.lineTo (endPt);

                g.strokePath (indicatorLine, juce::PathStrokeType (lineW));
            }
        }
    }
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