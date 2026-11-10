#include "Jack.h"
#include <utility>

void Jack::paint(juce::Graphics &g) {
    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillEllipse(getLocalBounds().toFloat());

    if (numConnections > 0) {
        g.setColour(colors.getColor(ThemeColors::tabBG));
        g.setFont(moduleFont);
        g.drawText(std::to_string(numConnections), getLocalBounds(), juce::Justification::centred);
    }
}

juce::String Jack::getName() {
    return jackName;
}

void Jack::mouseMove(const juce::MouseEvent &e) {
    auto mouseInGlobal = localPointToGlobal (e.position);

    if (onMouseMove) onMouseMove (e, mouseInGlobal, scrollDeltaY);
    scrollDeltaY = 0.f;
}

void Jack::mouseWheelMove(const juce::MouseEvent &e, const juce::MouseWheelDetails &wheel) {
    scrollDeltaY = wheel.deltaY;
    mouseMove(e);
}

void InJack::paint(juce::Graphics &g) {
    if (isHovering) {
        g.setColour(colors.getColor(ThemeColors::input));
    } else if (isConnecting) {
        g.setColour(colors.getColor(ThemeColors::neutral));
    } else {
        g.setColour(colors.getColor(ThemeColors::canvasBG));
    }

    g.fillEllipse(getLocalBounds().toFloat());
}

void OutJack::mouseDown (const juce::MouseEvent& e) {
    if (onDragStart) onDragStart (*this, e);
}

void OutJack::mouseDrag (const juce::MouseEvent& e) {
    auto mouseInGlobal = localPointToGlobal (e.position);

    if (onDrag) onDrag (mouseInGlobal);
}

void OutJack::mouseUp (const juce::MouseEvent& e) {
    if (onDragEnd) onDragEnd (e);
}