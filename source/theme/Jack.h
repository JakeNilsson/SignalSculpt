#ifndef JACK_H
#define JACK_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <utility>
#include "Colors.h"
#include "../helpers/fontHelper.h"

class OutJack;

class Jack : public juce::Component {
public:
    Jack(Colors &colorsRef, juce::String name) : colors(colorsRef), jackName(std::move(name)){}

    void paint(juce::Graphics &g) override;

    juce::String getName();

    void setID(int newID) {ID = newID;}
    [[nodiscard]] int  getID() const {return ID;}

    void setPort(int newPort) {port = newPort;}
    [[nodiscard]] int  getPort() const {return port;}

    void setHovering (const bool shouldHover) {
        if (isHovering != shouldHover) { isHovering = shouldHover; repaint(); }
    }
    [[nodiscard]] bool getHovering () const { return isHovering; }

    void mouseMove(const juce::MouseEvent &event) override;
    void mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) override;

    [[nodiscard]] int getNumConnections() const { return numConnections; }
    void incrementNumConnections() { ++numConnections; }
    void decrementNumConnections() { if (numConnections != 0) --numConnections; else jassertfalse; }

    std::function<void (const juce::MouseEvent &e, juce::Point<float> mousePos, float scrollDeltaY)> onMouseMove;

protected:
    Colors &colors;
    juce::String jackName;

    int ID = -1;
    int port = -1;

    bool isHovering = false;
    float scrollDeltaY = 0.f;

    int numConnections = 0;
    juce::Font moduleFont { makeKantumruyFont(32.f) };
};

class InJack : public Jack {
public:
    InJack(Colors &colorsRef, const juce::String &name) : Jack(colorsRef, name) {}

    void paint(juce::Graphics &g) override;

    void setConnecting (const bool shouldHighlight) {
        if (isConnecting != shouldHighlight) { isConnecting = shouldHighlight; repaint(); }
    }

private:
    juce::OwnedArray<OutJack *> connections;

    bool isConnecting = false;
};

class OutJack : public Jack {
public:
    OutJack(Colors &colorsRef, const juce::String &name) : Jack(colorsRef, name) {}

    std::function<void (OutJack&, const juce::MouseEvent&)> onDragStart;
    std::function<void (juce::Point<float> mousePos)> onDrag;
    std::function<void (const juce::MouseEvent&)> onDragEnd;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent &e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
};

#endif