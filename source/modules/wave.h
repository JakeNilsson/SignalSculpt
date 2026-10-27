#pragma once
#include "module.h"
#include "../theme/Jack.h"
#include "../theme/Colors.h"
#include "../theme/SiScSlider.h"
#include "../theme/SiScComboBox.h"

class WaveModuleComponent : public ModuleComponent {
public:
    explicit WaveModuleComponent(Colors &colorsRef) :
        ModuleComponent(colorsRef, 2.f, 2.f, "Wave"),

        shapeType(colorsRef),
        shapeSpec(colorsRef),

        phase(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            juce::MathConstants<double>::twoPi,
            0,
            colorsRef,
            0,
            {0, juce::MathConstants<double>::halfPi, juce::MathConstants<double>::pi, ((3 * juce::MathConstants<double>::pi) / 2)},
            IndicatorType::circle),

        freq(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            20000,
            261.63, // middle C
            colorsRef,
            0,
            {},
            IndicatorType::none),

        mix(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            100,
            80,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        det(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            100,
            20,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        voices(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            1,
            16,
            1,
            colorsRef,
            1,
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
            IndicatorType::line) {
        shapeType.getInJack().setPort(0);
        shapeSpec.getInJack().setPort(1);
        phase.getInJack().setPort(2);
        freq.getInJack().setPort(3);
        mix.getInJack().setPort(4);
        det.getInJack().setPort(5);
        voices.getInJack().setPort(6);

        outJack.setPort(0);
    };

    void paint(juce::Graphics &g) override;

    void onPathsReady() override;

    void setHeaderSinPath();
    void setOscPath();

    void setVisualGradient();

    juce::Array<InJack*> getInJacks() override;

    void setJackIDs(int ID) override;

private:
    juce::Path headerSinPath;

    juce::Path oscWavePath;
    juce::Path oscWavePathClosed;
    juce::Rectangle<float> oscGradientRect;
    juce::ColourGradient oscGradient;

    juce::Label shapeLabel {"shape", "Shape:"};
    SiScComboBox shapeType;
    SiScComboBox shapeSpec;

    juce::Rectangle<float> jackBorder;

    juce::Label phaseLabel {"phase", "Phase"};
    juce::Rectangle<float> phaseLabelBounds;
    SiScSlider phase;

    juce::Label freqLabel {"frequency", "Freq"};
    juce::Rectangle<float> freqLabelBounds;
    SiScSlider freq; // frequency

    juce::Label mixLabel {"mix", "Mix"};
    juce::Rectangle<float> mixLabelBounds;
    SiScSlider mix;

    juce::Label detLabel {"detune", "Det"};
    juce::Rectangle<float> detLabelBounds;
    SiScSlider det;  // detune

    juce::Label voicesLabel {"voices", "Voices"};
    juce::Rectangle<float> voicesLabelBounds;
    SiScSlider voices;

    juce::Rectangle<float> oscBounds;
    juce::Rectangle<float> centerBounds;

    juce::Rectangle<float> IOBounds;
    juce::Rectangle<float> IOTextBounds;
    juce::Path IOPath;

    bool prevState = true;
};
