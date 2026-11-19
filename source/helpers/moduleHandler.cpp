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

void ModuleHandler::removeModuleComponent (const int id, ConnectionOverlay& overlay) const {
    int index = -1;
    for (int i = 0; i < moduleComponents.size(); ++i)
        if (moduleComponents[i]->getModuleID() == id) { index = i; break; }

    if (index < 0) { jassertfalse; return; }

    auto* mod = moduleComponents[index];

    overlay.removeConnectionsForModule (id);
    overlay.removeInJacks (mod->getInJacks());

    moduleComponents.remove (index);
}
