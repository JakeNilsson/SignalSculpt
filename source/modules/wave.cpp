#include "wave.h"

/*void WaveModule::process(juce::AudioBuffer<float> &buffer) {

}

juce::String WaveModule::getName() const {
    return name;
}*/

void WaveModuleComponent::paint(juce::Graphics &g) {
    auto mousePos = getMouseXYRelative();
    drawStateButton(g, mousePos.toFloat());

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillPath(headerPath);

    g.setColour(statusColor);
    g.setFont(moduleFont);
    g.drawText(name.getText(), headerBounds, juce::Justification::centred);

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(body);
}
