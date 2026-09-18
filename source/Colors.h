#pragma once
#include <array>
#include <juce_gui_basics/juce_gui_basics.h>

enum class ThemeColors : size_t{
    tabBG,
    canvasBG,
    invisCanvasBG, // used for shadow gradients
    input,
    output,
    neutral,
};

class Colors {
public:
    [[nodiscard]] juce::Colour getColor(ThemeColors color) const {
        return colors[static_cast<size_t>(color)];
    }

    void changeUIColors(ThemeColors color, const juce::Colour& newColor) {
        colors[static_cast<size_t>(color)] = newColor;
    }

private:
    std::array<juce::Colour,6> colors = { // default dark theme colors
        juce::Colour{0xFF434242},
        juce::Colour{0xFF2B2B2B},
        juce::Colour{0x002B2B2B},
        juce::Colour{0xFF4EB0FF},
        juce::Colour{0xFFFF964B},
        juce::Colour{0xFFD9D9D9},
    };
};
