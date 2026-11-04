#include "../helpers/moduleHandler.h"
#include "../modules/wave.h"
#include "../modules/env.h"

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
        } case ModuleType::Envelope : {
            const auto envComponent = new EnvModuleComponent(colors);

            envComponent->setJackIDs(counter);

            moduleComponents.add(envComponent);
            break;
        }
        default : { // unimplemented, is it necessary?

        }
    }
}
