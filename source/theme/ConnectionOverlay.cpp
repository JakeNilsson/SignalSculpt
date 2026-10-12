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

    for (const auto& connection : connections) {
        g.setColour(connection.connectionColor);

        g.fillEllipse(connection.startPt.getX() - (pointSize / 2),
                      connection.startPt.getY() - (pointSize / 2),
                      pointSize, pointSize);

        g.strokePath(connection.connectionPath, juce::PathStrokeType(2.f));

        g.fillEllipse(connection.endPt.getX() - (pointSize / 2),
                  connection.endPt.getY() - (pointSize / 2),
                  pointSize,
                  pointSize);
    }
}

void ConnectionOverlay::beginConnection(OutJack &outJack, bool isInCanvas) {
    isDragging = true;
    liveConnection.isStartInCanvas = isInCanvas;

    if (isInCanvas) {
        const auto jackCenterInGlobal = outJack.localPointToGlobal(outJack.getLocalBounds().getCentre().toFloat());
        liveConnection.startPt = getLocalPoint (nullptr, jackCenterInGlobal);
    } else {
        //liveConnection.startPt = outJack.localCentreToGlobal();
    }

    liveConnection.isConnectionShown = true;
    liveConnection.connectionColor = colors.getColor(ThemeColors::visuals);

    for (const auto inJack : inJacks) {
        inJack->setConnecting(true);
    }

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

    const auto cursorScreen = cursorLoc.roundToInt();

    for (const auto inJack : inJacks)
        inJack->setHovering(inJack->getScreenBounds().contains(cursorScreen));

    repaint();
}

void ConnectionOverlay::cancelConnection() {
    isDragging = false;
    liveConnection.isConnectionShown = false;
    liveConnection.isEndActive = false;
    liveConnection.connectionPath.clear();

    for (auto inJack : inJacks) {
        bool repeat = false;

        if (inJack->getHovering()) {
            liveConnection.endPt = getLocalPoint(nullptr, inJack->getScreenBounds().getCentre().toFloat());

            for (const auto& connection : connections) {
                if (connection.startPt == liveConnection.startPt &&
                    connection.endPt == liveConnection.endPt) {
                    liveConnection.connectionPath.clear();
                    liveConnection.isConnectionShown = false;
                    repaint();

                    repeat = true;
                }
            }

            if (repeat) {} else {
                liveConnection.connectionPath.startNewSubPath(liveConnection.startPt);
                liveConnection.connectionPath.lineTo(liveConnection.endPt);

                connections.push_back(liveConnection);
            }
        }

        inJack->setConnecting(false);
        inJack->setHovering(false);
    }

    repaint();
}
