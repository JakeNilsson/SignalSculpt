#ifndef JACK_H
#define JACK_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <utility>
#include "Colors.h"

class OutJack;

class Jack : public juce::Component {
public:
    Jack(Colors &colorsRef, juce::String name) : colors(colorsRef), jackName(std::move(name)) {}

    void paint(juce::Graphics &g) override;

    juce::String getName();

protected:
    Colors &colors;
    juce::String jackName;
    bool isConnecting = false;
    bool isHovering = false;
};

class InJack : public Jack {
public:
    InJack(Colors &colorsRef, const juce::String &name) : Jack(colorsRef, name) {}

private:
    juce::OwnedArray<OutJack *> connections;
};

class OutJack : public Jack {
public:
    OutJack(Colors &colorsRef, const juce::String &name, float &signal) : Jack(colorsRef, name), signal(signal) {}

    void mouseDrag(const juce::MouseEvent &event) override;

private:
    float &signal;
};

#endif