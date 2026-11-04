#pragma once
#include "module.h"
#include "waveDSP.h"
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
        shapeType.getInJack().setPort(WaveDSP::shapeTypeIn);
        shapeSpec.getInJack().setPort(WaveDSP::shapeSpecIn);
        phase.getInJack().setPort(WaveDSP::phaseIn);
        freq.getInJack().setPort(WaveDSP::freqIn);
        mix.getInJack().setPort(WaveDSP::mixIn);
        det.getInJack().setPort(WaveDSP::detIn);
        voices.getInJack().setPort(WaveDSP::voicesIn);

        outJack.setPort(0);
    };

    void drawBody(juce::Graphics &g) override;
    void drawOscVisual(juce::Graphics &g) const;
    void drawIOSection(juce::Graphics &g);

    void onPathsReady() override;

    void setHeaderDesignPath() override;
    void setOscPath();

    void setVisualGradient() override;

    void connectListener (juce::Slider& slider, int control) const;
    void connectListener (juce::ComboBox& comboBox, int control) const;

    juce::Array<InJack*> getInJacks() override;

    void setJackIDs(int ID) override;

private:
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
};
