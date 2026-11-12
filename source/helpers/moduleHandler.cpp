#include "../helpers/moduleHandler.h"
#include "../modules/wave.h"
#include "../modules/env.h"
#include "../modules/filter.h"

/*void ModuleHandler::addModule(Module *module) {
    modules.add(module);
}*/

void ModuleHandler::addModuleComponent(ModuleType moduleType) {
    ModuleComponent* newChild;

    switch (moduleType) {
        case ModuleType::None : {
            return;
        }
        case ModuleType::Wave : {
            newChild = new WaveModuleComponent(colors);
            break;
        } case ModuleType::Envelope : {
            newChild = new EnvModuleComponent(colors);
            break;
        } case ModuleType::Filter : {
            newChild = new FilterModuleComponent(colors);
            break;
        }
        default : { // unimplemented, is it necessary?
            jassertfalse;
            return;
        }
    }

    ++counter;

    newChild->setJackIDs(counter);
    moduleComponents.add(newChild);
}
