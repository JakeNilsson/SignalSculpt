#pragma once
#include "wave.h"
#include "../Colors.h"

enum class ModuleType {
    Wave,
};

class ModuleHandler {
public:
    ModuleHandler (Colors& colorsRef) : colors (colorsRef) {}

    /*void addModule (Module *module);*/
    void addModuleComponent (ModuleType moduleType, juce::Rectangle<float> clickBlock);

private:
    Colors &colors;

    /*juce::OwnedArray<Module> modules;*/
    juce::OwnedArray<ModuleComponent> moduleComponents;
};
