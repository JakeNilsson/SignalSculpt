#include "moduleHandler.h"

/*void ModuleHandler::addModule(Module *module) {
    modules.add(module);
}*/

void ModuleHandler::addModuleComponent(ModuleType moduleType) const {
    switch (moduleType) {
        case ModuleType::Wave : {
            const auto waveComponent = new WaveModuleComponent(colors);

            moduleComponents.add(waveComponent);
            break;
        }
        default : { // unimplemented, is it necessary?

        }
    }
}
