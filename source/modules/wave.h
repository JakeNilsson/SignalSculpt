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
    explicit WaveModuleComponent(Colors &colorsRef) : ModuleComponent(colorsRef) {
    };

    void paint(juce::Graphics &g) override;

    [[nodiscard]] float getBlockSizeX() const {
        return blockSizeX;
    }

    [[nodiscard]] float getBlockSizeY() const {
        return blockSizeY;
    }

private:
    float blockSizeX = 2;
    float blockSizeY = 2;
};