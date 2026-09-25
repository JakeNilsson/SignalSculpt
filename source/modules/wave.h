#pragma once
#include "module.h"
#include "../theme/Colors.h"
#include "../theme/SiScSlider.h"

/*class WaveModule : public Module {
public:
    void process (juce::AudioBuffer<float>& buffer) override;

    [[nodiscard]] juce::String getName() const override;

private:
    juce::String name{"Wave"};
};*/

class WaveModuleComponent : public ModuleComponent {
public:
    explicit WaveModuleComponent(Colors &colorsRef) :
        ModuleComponent(colorsRef, 2.f, 2.f, "Wave"),

        phase(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            juce::MathConstants<double>::twoPi,
            0,
            0,
            {0, juce::MathConstants<double>::halfPi, juce::MathConstants<double>::pi, ((3 * juce::MathConstants<double>::pi) / 2)},
            IndicatorType::circle),

        freq(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            20000,
            261.63, // middle C
            0,
            {},
            IndicatorType::none),

        mix(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            100,
            100,
            0,
            {},
            IndicatorType::none),

        det(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            100,
            20,
            0,
            {},
            IndicatorType::none),

        voices(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            1,
            16,
            1,
            1,
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
            IndicatorType::line)
    {};

    void paint(juce::Graphics &g) override;

    void onPathsReady() override;

    void setHeaderSinPath();
    void setOscPath();

    void setVisualGradient();

private:
    juce::Path headerSinPath;

    juce::Path oscWavePath;
    juce::Path oscWavePathClosed;
    juce::Rectangle<float> oscGradientRect;
    juce::ColourGradient oscGradient;

    juce::Label shapeLabel {"shape", "Shape:"};
    juce::ComboBox shapeType;
    juce::ComboBox shapeSpec;

    juce::Rectangle<float> jackBounds;
    juce::Rectangle<float> jackBorder;

    juce::Label phaseLabel {"phase", "Phase"};
    SiScSlider phase;

    juce::Label freqLabel {"frequency", "Freq"};
    SiScSlider freq; // frequency

    juce::Label mixLabel {"mix", "Mix"};
    SiScSlider mix;

    juce::Label detLabel {"detune", "Det"};
    SiScSlider det;  // detune

    juce::Label voicesLabel {"voices", "Voices"};
    SiScSlider voices;

    juce::Rectangle<float> oscBounds;
    juce::Rectangle<float> centerBounds;

    juce::Rectangle<float> IOBounds;
    juce::Rectangle<float> IOTextBounds;
    juce::Path IOPath;

    bool prevState = true;
};