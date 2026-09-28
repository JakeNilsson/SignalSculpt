#include "Jack.h"
#include <utility>

void Jack::paint(juce::Graphics &g) {
    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillEllipse(getLocalBounds().toFloat());
}

juce::String Jack::getName() {
    return jackName;
}

void OutJack::mouseDrag(const juce::MouseEvent &event) {

}