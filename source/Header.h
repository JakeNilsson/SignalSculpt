#pragma once
#include <cmath>
#include "theme/Colors.h"
#include "theme/SiScLookAndFeel.h"
#include <juce_audio_devices/juce_audio_devices.h>

class Header : public juce::Component {
public:
    explicit Header(Colors &colorsRef) : midiInputList(colorsRef), colors(colorsRef) {
        midiInputList.setName("MIDI Inputs:");
    }

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

        g.setColour(colors.getColor(ThemeColors::neutral));
        g.setFont(settingsFont);
        g.drawText(midiInputList.getName(),
                   midiInputTextBounds,
                   juce::Justification::centredTop,
                   false);
    };

    bool hitTest (int x, int y) override
    {
        return headerPath.contains (static_cast<float>(x), static_cast<float>(y));
    }

    void setTabBounds() {
        headerPath.clear();
        headerDividerPath.clear();

        const auto bounds = getLocalBounds().toFloat();
        const auto width = bounds.getWidth();
        const auto height = bounds.getHeight();
        const auto headerHeight = height / 2.f;

        const auto tabWidth = width * 1/4;
        constexpr auto handleHeightRatio = 5.f/8.f;

        const auto handleHeight = headerHeight * handleHeightRatio;
        const auto tabHandleDiff = headerHeight - handleHeight;

        const auto midiInputWidth = tabWidth / 2;

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
        juce::FontVariableSetting regularSetting[] {
            { juce::FontFeatureTag("wght"), 400.0f }
        };

        auto semiboldTypeface = baseTypeface->cloneWithVariableSettings(semiboldSetting);
        auto regularTypeface = baseTypeface->cloneWithVariableSettings(regularSetting);

        auto headerFontOptions = juce::FontOptions(semiboldTypeface).withPointHeight(64.0f);
        auto bulletFontOptions = juce::FontOptions(semiboldTypeface).withPointHeight(48.0f);
        auto settingsFontOptions = juce::FontOptions(regularTypeface).withPointHeight(16.f);
        headerFont = juce::Font(headerFontOptions);
        bulletPointFont = juce::Font(bulletFontOptions);
        settingsFont = juce::Font(settingsFontOptions);

        midiInputBounds.setBounds(tabWidth + tabHandleDiff, 0, midiInputWidth, handleHeight);

        midiInputsListBounds = midiInputBounds;
        midiInputsListBounds.removeFromTop(35);
        midiInputsListBounds.removeFromBottom(10);
        midiInputList.setBounds(midiInputsListBounds.toNearestInt());
        addAndMakeVisible(midiInputList);

        int id = 1;
        midiInputList.clear();
        for (const auto& device : juce::MidiInput::getAvailableDevices()) {
            midiInputList.addItem(device.name, id);
            id++;
        }

        midiInputTextBounds = midiInputBounds;
        midiInputTextBounds.removeFromTop(10);
        midiInputTextBounds.removeFromBottom(35);
    }

private:
    juce::Path headerPath;
    juce::Path headerDividerPath;

    juce::Rectangle<float> headerShadow;
    juce::Rectangle<float> headerTab;

    juce::Rectangle<float> midiInputBounds;
    juce::Rectangle<float> midiInputsListBounds;
    juce::Rectangle<float> midiInputTextBounds;

    juce::Font headerFont;
    juce::Font bulletPointFont;
    juce::Font settingsFont;

    SiScComboBox midiInputList;
    juce::AudioDeviceManager deviceManager;

    Colors &colors;
};