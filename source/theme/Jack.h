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

    void setID(int newID) {ID = newID;}
    [[nodiscard]] int  getID() const {return ID;}

    void setPort(int newPort) {port = newPort;}
    [[nodiscard]] int  getPort() const {return port;}

protected:
    Colors &colors;
    juce::String jackName;

    int ID = -1;
    int port = -1;
};

class InJack : public Jack {
public:
    InJack(Colors &colorsRef, const juce::String &name) : Jack(colorsRef, name) {}

    void paint(juce::Graphics &g) override;

    void setConnecting (const bool shouldHighlight) {
        if (isConnecting != shouldHighlight) { isConnecting = shouldHighlight; repaint(); }
    }
    void setHovering (const bool shouldHover) {
        if (isHovering != shouldHover) { isHovering = shouldHover; repaint(); }
    }
    [[nodiscard]] bool getHovering () const { return isHovering; }

private:
    juce::OwnedArray<OutJack *> connections;

    bool isConnecting = false;
    bool isHovering = false;
};

class OutJack : public Jack {
public:
    OutJack(Colors &colorsRef, const juce::String &name) : Jack(colorsRef, name) {}

    std::function<void (OutJack&, const juce::MouseEvent&)> onDragStart;
    std::function<void (OutJack&, const juce::Point<float> mousePos)> onDrag;
    std::function<void (OutJack&, const juce::MouseEvent&)> onDragEnd;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent &e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
};

#endif