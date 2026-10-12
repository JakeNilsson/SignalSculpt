#include "ConnectionOverlay.h"

void ConnectionOverlay::paint(juce::Graphics &g) {
    if (liveConnection.isConnectionShown) {
        g.setColour(liveConnection.connectionColor);

        g.fillEllipse(liveConnection.startPt.getX() - (pointSize / 2),
                      liveConnection.startPt.getY() - (pointSize / 2),
                      pointSize, pointSize);

        g.strokePath(liveConnection.connectionPath, juce::PathStrokeType(2.f));

        if (liveConnection.isEndActive) {
            g.fillEllipse(liveConnection.endPt.getX() - (pointSize / 2),
                      liveConnection.endPt.getY() - (pointSize / 2),
                      pointSize,
                      pointSize);
        }
    }
}

void ConnectionOverlay::beginConnection(OutJack &outJack, bool isInCanvas) {
    isDragging = true;
    liveConnection.isStartInCanvas = isInCanvas;

    if (isInCanvas) {
        auto jackCenterInGlobal = outJack.localPointToGlobal(outJack.getLocalBounds().getCentre().toFloat());
        liveConnection.startPt = getLocalPoint (nullptr, jackCenterInGlobal);
    } else {
        //liveConnection.startPt = outJack.localCentreToGlobal();
    }

    liveConnection.isConnectionShown = true;
    liveConnection.connectionColor = colors.getColor(ThemeColors::visuals);

    repaint();
}

void ConnectionOverlay::updateConnection(juce::Point<float> cursorLoc) {
    liveConnection.connectionPath.clear();

    if (liveConnection.isStartInCanvas) {
        liveConnection.endPt = getLocalPoint (nullptr, cursorLoc);
    } else {
        liveConnection.endPt = cursorLoc;
    }

    liveConnection.connectionPath.startNewSubPath(liveConnection.startPt);
    liveConnection.connectionPath.lineTo(liveConnection.endPt);

    liveConnection.isEndInCanvas = false;
    liveConnection.isEndActive = true;

    repaint();
}

void ConnectionOverlay::cancelConnection() {
    isDragging = false;
    liveConnection.isConnectionShown = false;
    liveConnection.isEndActive = false;
    liveConnection.connectionPath.clear();

    repaint();
}
