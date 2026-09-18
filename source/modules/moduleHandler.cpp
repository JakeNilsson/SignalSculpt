#include "moduleHandler.h"

/*void ModuleHandler::addModule(Module *module) {
    modules.add(module);
}*/

void ModuleHandler::addModuleComponent(ModuleType moduleType, juce::Rectangle<float> clickBlock) {
    switch (moduleType) {
        case ModuleType::Wave : {
            auto waveComponent = new WaveModuleComponent(colors);

            clickBlock.expand(clickBlock.getWidth() * waveComponent->getBlockSizeX() - 1,
                              clickBlock.getHeight() * waveComponent->getBlockSizeY() - 1);

            waveComponent->setBounds(clickBlock.toNearestInt());
            waveComponent->repaint();

            moduleComponents.add(waveComponent);
            break;
        }
        default : {
            DBG("UNSELECTED");
        }
    }
}
