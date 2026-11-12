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

        signalInJack(colorsRef, "Filter Signal In"){

        filterType.getInJack().setPort(FilterDSP::filterTypeIn);
        filterSpec.getInJack().setPort(FilterDSP::filterSpecIn);

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

    void connectListener (juce::Slider& slider, int control) const;
    void connectListener (juce::ComboBox& comboBox, int control) const;

    juce::Array<InJack*> getInJacks() override;

    void drawFilterVisual(juce::Graphics& g) const;

private:
    juce::Path oscWavePath;
    juce::Path oscWavePathClosed;
    juce::Rectangle<float> oscGradientRect;
    juce::ColourGradient oscGradient;

    juce::Label filterLabel {"filter", "Filter:"};
    SiScComboBox filterType;
    SiScComboBox filterSpec;

    juce::Path IOPath;
    juce::Rectangle<float> IOTextBounds;
    juce::Rectangle<float> jackBorder;

    juce::Label inJackLabel {"signal in", "Signal"};
    juce::Rectangle<float> inJackLabelBounds;
    InJack signalInJack;

    juce::Rectangle<float> oscBounds;
    juce::Rectangle<float> centerBounds;
    juce::Rectangle<float> IOBounds;
};