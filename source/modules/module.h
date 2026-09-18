#pragma once
#include "../Colors.h"

/*class Module {
public:
    virtual ~Module() = default;

    virtual void process (juce::AudioBuffer<float>& buffer) = 0;
    [[nodiscard]] virtual juce::String getName() const = 0;
};*/

class ModuleComponent : public juce::Component {
public:
    ModuleComponent(Colors &colorsRef) :  colors(colorsRef) {}

    void paint(juce::Graphics& g) override = 0;

protected:
    Colors &colors;
};