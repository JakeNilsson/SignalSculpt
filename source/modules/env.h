#pragma once
#include "module.h"
#include "../theme/Jack.h"
#include "../theme/Colors.h"
#include "../theme/SiScSlider.h"
#include "../theme/SiScComboBox.h"
#include "envDSP.h"

class EnvModuleComponent : public ModuleComponent {
public:
    explicit EnvModuleComponent(Colors& colorsRef) :
        ModuleComponent(colorsRef, 2.f, 2.f, "Envelope"),

        delay(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            4,
            0,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        attack(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            32,
            0,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        hold(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            4,
            0,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        decay(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            32,
            1,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        sustain(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            1,
            1,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        release(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            0,
            32,
            0.089,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        triggerInJack(colorsRef, "Gate In")
    {
        delay.getInJack().setPort(EnvDSP::delayIn);
        attack.getInJack().setPort(EnvDSP::attackIn);
        hold.getInJack().setPort(EnvDSP::holdIn);
        triggerInJack.setPort(EnvDSP::gateIn);
        decay.getInJack().setPort(EnvDSP::decayIn);
        sustain.getInJack().setPort(EnvDSP::sustainIn);
        release.getInJack().setPort(EnvDSP::releaseIn);

        outJack.setPort(EnvDSP::envOut);
    }

    void drawBody(juce::Graphics &g) override;
    void drawEnvVisual(juce::Graphics &g) const;
    void drawIOSection(juce::Graphics &g);

    void onPathsReady() override;

    void connectListener(SiScSlider& slider, int control);

    void setHeaderDesignPath() override;
    void setEnvPath();
    void setVisualGradient() override;
    void setControlBounds(std::vector<juce::Rectangle<float>>) override;
    void setControlAndLabelVisAndBounds(SiScSlider& control, juce::Rectangle<float>& labelBounds, float x, float y, float skewFactor = 1);

    juce::Array<InJack*> getInJacks() override;

    void setJackIDs(int ID) override;

private:
    juce::Path envPath;
    juce::Path envPathClosed;

    juce::Rectangle<float> envBounds;
    juce::Path envPathClip;
    juce::Rectangle<float> innerEnvBounds;

    juce::Rectangle<float> envGradientRect;
    juce::ColourGradient envGradient;

    double envVisualLength;

    juce::Rectangle<float> IOBounds;
    juce::Rectangle<float> IOTextBounds;
    juce::Path IOPath;

    juce::Label delayLabel {"delay", "Del"};
    juce::Rectangle<float> delayLabelBounds;
    SiScSlider delay;

    juce::Label attackLabel {"attack", "Atk"};
    juce::Rectangle<float> attackLabelBounds;
    SiScSlider attack;

    juce::Label holdLabel {"hold", "Hld"};
    juce::Rectangle<float> holdLabelBounds;
    SiScSlider hold;

    juce::Label decayLabel {"decay", "Dec"};
    juce::Rectangle<float> decayLabelBounds;
    SiScSlider decay;

    juce::Label sustainLabel {"sustain", "Sus"};
    juce::Rectangle<float> sustainLabelBounds;
    SiScSlider sustain;

    juce::Label releaseLabel {"release", "Rel"};
    juce::Rectangle<float> releaseLabelBounds;
    SiScSlider release;

    juce::Label inJackLabel {"gate in", "Gate"};
    juce::Rectangle<float> inJackLabelBounds;
    InJack triggerInJack;

    juce::Rectangle<float> jackBorder; // for output jack, not the in-jack above
};