#pragma once
#include "module.h"
#include "../theme/Jack.h"
#include "../theme/Colors.h"
#include "../theme/SiScSlider.h"
#include "../theme/SiScComboBox.h"

class EnvModuleComponent : public ModuleComponent {
public:
    explicit EnvModuleComponent(Colors& colorsRef) :
        ModuleComponent(colorsRef, 2.f, 2.f, "Envelope") {

        outJack.setPort(0);
    }

    void drawBody(juce::Graphics &g) override;
    void drawEnvVisual(juce::Graphics &g) const;
    void drawIOSection(juce::Graphics &g) const;

    void onPathsReady() override;

    void setHeaderDesignPath() override;
    void setEnvPath();

    void setVisualGradient() override; //test

    juce::Array<InJack*> getInJacks() override;

    void setJackIDs(int ID) override;

private:
    juce::Path envPath;
    juce::Path envPathClosed;

    juce::Rectangle<float> envBounds;

    juce::Rectangle<float> envGradientRect;
    juce::ColourGradient envGradient;

    juce::Rectangle<float> IOBounds;
    juce::Rectangle<float> IOTextBounds;
    juce::Path IOPath;

    juce::Rectangle<float> jackBorder;
};