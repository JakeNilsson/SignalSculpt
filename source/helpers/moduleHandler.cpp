#include "../helpers/moduleHandler.h"

/*void ModuleHandler::addModule(Module *module) {
    modules.add(module);
}*/

void ModuleHandler::addModuleComponent(ModuleType moduleType) {
    counter++;

    switch (moduleType) {
        case ModuleType::Wave : {
            const auto waveComponent = new WaveModuleComponent(colors);

            waveComponent->setJackIDs(counter);

            moduleComponents.add(waveComponent);
            break;
        }
        default : { // unimplemented, is it necessary?

        }
    }
}
