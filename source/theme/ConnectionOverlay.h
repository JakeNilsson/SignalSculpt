#ifndef SIGNALSCULPT_CONNECTIONOVERLAY_H
#define SIGNALSCULPT_CONNECTIONOVERLAY_H

#include "Jack.h"
#include "Colors.h"
#include "../PluginProcessor.h"

struct Connection {
    juce::Point<float> startPt;
    int startJackID = 0;
    int startJackPort = 0;
    bool isStartInCanvas;

    juce::Point<float> endPt;
    int endJackID = 0;
    int endJackPort = 0;
    bool isEndInCanvas;
    bool isEndActive;

    juce::Path connectionPath;
    bool isConnectionShown;

    juce::Colour connectionColor;
};

class ConnectionOverlay : public juce::Component {
public:
    ConnectionOverlay(Colors& colorsRef,
                      juce::Viewport& canvasContainerRef,
                      juce::AudioProcessorEditor& editorRef) : colors(colorsRef),
                                                               canvasContainer(canvasContainerRef),
                                                               editor(editorRef) {}

    void paint(juce::Graphics &g) override;

    void beginConnection(OutJack& outJack, bool isInCanvas);
    void updateConnection(juce::Point<float> cursorLoc);
    void cancelConnection();
    void endConnection(InJack& inJack, bool isInCanvas);

    juce::Array<InJack*>& getInJacks() {
        return inJacks;
    }

    void addInJacks(const juce::Array<InJack*>& newInJacks) {
        inJacks.addArray(newInJacks);
    }

    std::function<void (int, int, int, int)> connectDSP;

private:
    bool isDragging = false;
    float pointSize = 8;
    Connection liveConnection;
    std::vector<Connection> connections;
    juce::Array<InJack*> inJacks;
    Colors& colors;
    juce::Viewport& canvasContainer;
    juce::AudioProcessorEditor& editor;
};


#endif
