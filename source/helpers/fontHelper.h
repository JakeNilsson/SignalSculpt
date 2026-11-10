#pragma once
#include <juce_graphics/juce_graphics.h>
#include <BinaryData.h>

inline juce::Font makeKantumruyFont (const float pointHeight)
{
    static const auto regularTypeface = []
    {
        auto baseTypeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::kantumruyProVar_ttf,
            BinaryData::kantumruyProVar_ttfSize);

        constexpr juce::FontVariableSetting regularSetting[] {
            { juce::FontFeatureTag ("wght"), 400.0f }
        };

        return baseTypeface->cloneWithVariableSettings (regularSetting);
    }();

    return juce::Font (
        juce::FontOptions (regularTypeface).withPointHeight (pointHeight));
}