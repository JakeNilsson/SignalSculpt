#ifndef SIGNALSCULPT_CONNECTIONOVERLAY_H
#define SIGNALSCULPT_CONNECTIONOVERLAY_H

#include "Jack.h"
#include "Colors.h"
#include "SiScSlider.h"
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
                      juce::AudioProcessorEditor& editorRef) : globalWatcher(*this),
                                                               colors(colorsRef),
                                                               canvasContainer(canvasContainerRef),
                                                               editor(editorRef),
                                                               modulationSlider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                               SliderType::modulation,
                                                               -1,
                                                               1,
                                                               1,
                                                               colorsRef,
                                                               0,
                                                               {},
                                                               IndicatorType::none)
    {
        setWantsKeyboardFocus(true);

        addAndMakeVisible(modulationSlider);
        modulationSlider.setVisible(false);
        modulationSlider.setVelocityModeParameters (1.0,    // sensitivity
                                                    1,      // threshold
                                                    0.0,    // offset
                                                    false); // userCanPressKeyToSwapMode

        modulationSlider.onValueChange = [this]
        {
            for (auto* c : highlightedConnections)
                if (c->isSelected)
                    c->modAmount = modulationSlider.getValue();
        };

        juce::Desktop::getInstance().addGlobalMouseListener(&globalWatcher);
    }

    ~ConnectionOverlay() override
    {
        juce::Desktop::getInstance().removeGlobalMouseListener(&globalWatcher);
    }

    void paint(juce::Graphics &g) override;

    void beginConnection(OutJack& outJack, bool isInCanvas);
    void updateConnection(juce::Point<float> cursorLoc);
    void endConnection();
    static void removeConnection(const Connection& connectionToRemove, std::vector<Connection>& connectionList);

    bool keyPressed(const juce::KeyPress &key) override;
    void modifierKeysChanged (const juce::ModifierKeys& mods) override;
    void clearHighlights();

    juce::Array<InJack*>& getInJacks() {
        return inJacks;
    }

    void addInJacks(const juce::Array<InJack*>& newInJacks) {
        inJacks.addArray(newInJacks);
    }

    std::function<void (int, int, int, int)> connectDSP;
    std::function<void (int, int, int, int)> disconnectDSP;

    void handleHover(juce::ModifierKeys mods, juce::Point<float> screenPos, float scrollDeltaY);

private:
    void rebuildHighlights();

    struct GlobalWatcher : juce::MouseListener {
        explicit GlobalWatcher(ConnectionOverlay& o) : owner(o) {}

        void mouseMove(const juce::MouseEvent& e) override {
            owner.handleHover(e.mods, e.getScreenPosition().toFloat(), 0.f);
        }
        void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override {
            owner.handleHover(e.mods, e.getScreenPosition().toFloat(), w.deltaY);
        }

        ConnectionOverlay& owner;
    };

    GlobalWatcher globalWatcher;

    Jack* hoveredJack = nullptr;
    bool isDragging = false;
    bool ctrlWasDown = false;
    float pointSize = 8;
    int selection = 0;
    Connection liveConnection;
    std::vector<Connection> connections;
    std::vector<Connection*> highlightedConnections;
    juce::Array<InJack*> inJacks;
    Colors& colors;
    juce::Viewport& canvasContainer;
    juce::AudioProcessorEditor& editor;

    SiScSlider modulationSlider;
};


#endif
