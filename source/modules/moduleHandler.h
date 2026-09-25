#pragma once
#include "wave.h"
#include "../theme/Colors.h"

enum class ModuleType {
    Wave,
};

class ModuleHandler {
public:
    ModuleHandler (Colors& colorsRef, juce::OwnedArray<ModuleComponent>& moduleComponentsRef) :
        colors (colorsRef),
        moduleComponents (moduleComponentsRef) {}

    /*void addModule (Module *module);*/
    void addModuleComponent (ModuleType moduleType) const;

private:
    Colors &colors;

    /*juce::OwnedArray<Module> modules;*/
    juce::OwnedArray<ModuleComponent> &moduleComponents;
};
