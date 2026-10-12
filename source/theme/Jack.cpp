#include "Jack.h"
#include <utility>

void Jack::paint(juce::Graphics &g) {
    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillEllipse(getLocalBounds().toFloat());
}

juce::String Jack::getName() {
    return jackName;
}

void OutJack::mouseDown (const juce::MouseEvent& e) {
    if (onDragStart) onDragStart (*this, e);
}

void OutJack::mouseDrag (const juce::MouseEvent& e) {
    auto mouseInGlobal = localPointToGlobal (e.position);

    if (onDrag) onDrag (*this, mouseInGlobal);
}

void OutJack::mouseUp (const juce::MouseEvent& e) {
    if (onDragEnd) onDragEnd (*this, e);
}