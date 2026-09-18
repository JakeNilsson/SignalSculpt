#include "wave.h"

/*void WaveModule::process(juce::AudioBuffer<float> &buffer) {

}

juce::String WaveModule::getName() const {
    return name;
}*/

void WaveModuleComponent::paint(juce::Graphics &g) {
    const auto bounds = getLocalBounds();
    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(bounds);
}
