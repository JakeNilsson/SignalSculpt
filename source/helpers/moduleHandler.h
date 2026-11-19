#pragma once
#include "../theme/Colors.h"
#include "../modules/module.h"
#include "moduleIDs.h"
#include "../theme/ConnectionOverlay.h"

class ModuleHandler {
public:
    ModuleHandler (Colors& colorsRef, juce::OwnedArray<ModuleComponent>& moduleComponentsRef) :
        colors (colorsRef),
        moduleComponents (moduleComponentsRef) {}

    void addModuleComponent (ModuleType moduleType);
    void removeModuleComponent (int id, ConnectionOverlay& overlay) const;

private:
    Colors &colors;

    /*juce::OwnedArray<Module> modules;*/
    juce::OwnedArray<ModuleComponent> &moduleComponents;

    int counter = std::to_underlying(StaticIDs::StaticCount);
};
