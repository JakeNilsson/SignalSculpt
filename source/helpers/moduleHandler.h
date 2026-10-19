#pragma once
#include "../modules/wave.h"
#include "../theme/Colors.h"
#include "moduleIDs.h"

class ModuleHandler {
public:
    ModuleHandler (Colors& colorsRef, juce::OwnedArray<ModuleComponent>& moduleComponentsRef) :
        colors (colorsRef),
        moduleComponents (moduleComponentsRef) {}

    /*void addModule (Module *module);*/
    void addModuleComponent (ModuleType moduleType);

private:
    Colors &colors;

    /*juce::OwnedArray<Module> modules;*/
    juce::OwnedArray<ModuleComponent> &moduleComponents;

    int counter = 0;
};
