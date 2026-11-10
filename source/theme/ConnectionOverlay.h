#ifndef SIGNALSCULPT_CONNECTIONOVERLAY_H
#define SIGNALSCULPT_CONNECTIONOVERLAY_H

#include "Jack.h"
#include "Colors.h"
#include "../PluginProcessor.h"

struct Connection {
    juce::Point<float> startPt;
    OutJack* startJack;
    bool isStartInCanvas;

    juce::Point<float> endPt;
    InJack* endJack;
    bool isEndInCanvas;
    bool isEndActive;

    double modAmount = 1.00; // can only be set from -1 to 1, defaults to positive max
    bool isBipolar = false;

    juce::Path connectionPath;
    bool isConnectionShown;
    bool isSelected = false;

    juce::Colour connectionColor;
};

class ConnectionOverlay : public juce::Component {
public:
    ConnectionOverlay(Colors& colorsRef,
                      juce::Viewport& canvasContainerRef,
                      juce::AudioProcessorEditor& editorRef) : colors(colorsRef),
                                                               canvasContainer(canvasContainerRef),
                                                               editor(editorRef)
    {
        setWantsKeyboardFocus(true);
    }

    void paint(juce::Graphics &g) override;

    void mouseMove(const juce::MouseEvent &e, const juce::Point<float> mousePos, float scrollDeltaY);

    void beginConnection(OutJack& outJack, bool isInCanvas);
    void updateConnection(juce::Point<float> cursorLoc);
    void endConnection();
    static void removeConnection(const Connection& connectionToRemove, std::vector<Connection>& connectionList);

    bool keyPressed(const juce::KeyPress &key) override;

    juce::Array<InJack*>& getInJacks() {
        return inJacks;
    }

    void addInJacks(const juce::Array<InJack*>& newInJacks) {
        inJacks.addArray(newInJacks);
    }

    std::function<void (int, int, int, int)> connectDSP;
    std::function<void (int, int, int, int)> disconnectDSP;

private:
    bool isDragging = false;
    bool ctrlWasDown = false;
    float pointSize = 8;
    int selection = 0;
    Connection liveConnection;
    std::vector<Connection> connections;
    std::vector<Connection> highlightedConnections;
    juce::Array<InJack*> inJacks;
    Colors& colors;
    juce::Viewport& canvasContainer;
    juce::AudioProcessorEditor& editor;
};


#endif
