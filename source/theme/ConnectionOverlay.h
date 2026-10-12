#ifndef SIGNALSCULPT_CONNECTIONOVERLAY_H
#define SIGNALSCULPT_CONNECTIONOVERLAY_H

#include "Jack.h"
#include "Colors.h"

struct Connection {
    juce::Point<float> startPt;
    bool isStartInCanvas;

    juce::Point<float> endPt;
    bool isEndInCanvas;
    bool isEndActive;

    juce::Path connectionPath;
    bool isConnectionShown;

    juce::Colour connectionColor;
};

class ConnectionOverlay : public juce::Component {
public:
    ConnectionOverlay(Colors& colorsRef,
                      juce::Viewport& canvasContainerRef) : colors(colorsRef),
                                                            canvasContainer(canvasContainerRef) {}

    void paint(juce::Graphics &g) override;

    void beginConnection(OutJack& outJack, bool isInCanvas);
    void updateConnection(juce::Point<float> cursorLoc);
    void cancelConnection();
    void endConnection(InJack& inJack, bool isInCanvas);

private:
    bool isDragging = false;
    float pointSize = 8;
    Connection liveConnection;
    juce::OwnedArray<Connection> connections;
    juce::OwnedArray<InJack*> inJacks;
    Colors& colors;
    juce::Viewport& canvasContainer;
};


#endif
