#pragma once
#include "module.h"
#include "../Colors.h"

/*class WaveModule : public Module {
public:
    void process (juce::AudioBuffer<float>& buffer) override;

    [[nodiscard]] juce::String getName() const override;

private:
    juce::String name{"Wave"};
};*/

class WaveModuleComponent : public ModuleComponent {
public:
    explicit WaveModuleComponent(Colors &colorsRef) : ModuleComponent(colorsRef, 2.f, 2.f, "Wave") {

    };

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

    juce::Label phaseLabel {"phase", "Phase"};
    juce::Slider phase;

    juce::Label freqLabel {"frequency", "Freq"};
    juce::Slider freq; // frequency

    juce::Label mixLabel {"mix", "Mix"};
    juce::Slider mix;

    juce::Label detLabel {"detune", "Det"};
    juce::Slider det;  // detune

    juce::Label voicesLabel {"voices", "Voices"};
    juce::Slider voices;

    juce::Rectangle<float> oscBounds;
    juce::Rectangle<float> centerBounds;

    juce::Rectangle<float> IOBounds;
    juce::Rectangle<float> IOTextBounds;
    juce::Path IOPath;

    bool prevState = true;
};