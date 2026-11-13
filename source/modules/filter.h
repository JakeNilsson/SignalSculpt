#pragma once

#include "module.h"
#include "filterDSP.h"
#include "../theme/SiScComboBox.h"
#include "../theme/SiScSlider.h"

class FilterModuleComponent : public ModuleComponent {
public:
    explicit FilterModuleComponent(Colors& colorsRef) :
        ModuleComponent(colorsRef, 2, 2, "Filter"),

        filterType(colorsRef),
        filterSpec(colorsRef),

        res(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            SliderType::standard,
            0.5,
            20,
            0.8,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        cut(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            SliderType::standard,
            8,
            22050,
            425,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        signalInJack(colorsRef, "Filter Signal In"),

        mix(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            SliderType::standard,
            0,
            100,
            100,
            colorsRef,
            0,
            {},
            IndicatorType::none),

        blend(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            SliderType::standard,
            0,
            1,
            0,
            colorsRef,
            0,
            {},
            IndicatorType::none){

        filterType.getInJack().setPort(FilterDSP::filterTypeIn);
        filterSpec.getInJack().setPort(FilterDSP::filterSpecIn);

        signalInJack.setPort(FilterDSP::signalIn);

        outJack.setPort(FilterDSP::filterOut);
    }

    void drawBody(juce::Graphics &) override;
    void drawIOSection(juce::Graphics &);
    void setFilterPath();
    void onPathsReady() override;

    void setVisualGradient() override;
    void setHeaderDesignPath() override;
    void setControlBounds(const std::vector<juce::Rectangle<float>> &) override;
    void setJackIDs(int ID) override;

    void connectListener (juce::Slider& slider, int control);
    void connectListener (juce::ComboBox& comboBox, int control);

    juce::Array<InJack*> getInJacks() override;

    void drawFilterVisual(juce::Graphics& g) const;

    static juce::NormalisableRange<double> logRange(double min, double max);

private:
    juce::Path filterWavePath;
    juce::Path filterWavePathClosed;

    juce::Label filterLabel {"filter", "Filter:"};
    SiScComboBox filterType;
    SiScComboBox filterSpec;

    juce::Rectangle<float> filterGradientRect;
    juce::ColourGradient filterGradient;

    juce::Path IOPath;
    juce::Rectangle<float> IOTextBounds;
    juce::Rectangle<float> jackBorder;

    juce::Label resLabel {"resonance", "Res"};
    juce::Rectangle<float> resLabelBounds;
    SiScSlider res;

    juce::Label cutLabel {"cutoff", "Cut"};
    juce::Rectangle<float> cutLabelBounds;
    SiScSlider cut;

    juce::Label inJackLabel {"signal in", "Signal"};
    juce::Rectangle<float> inJackLabelBounds;
    InJack signalInJack;

    juce::Label mixLabel {"mix", "Mix"};
    juce::Rectangle<float> mixLabelBounds;
    SiScSlider mix;

    juce::Label blendLabel {"blend", "Blend"};
    juce::Rectangle<float> blendLabelBounds;
    SiScSlider blend;

    juce::Rectangle<float> filterBounds;
    juce::Rectangle<float> centerBounds;
    juce::Rectangle<float> IOBounds;
};