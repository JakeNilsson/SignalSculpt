#include "ConnectionOverlay.h"
#include <algorithm>

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

    for (const auto& connection : highlightedConnections) {
        if (connection->isConnectionShown) {
            if (connection->isSelected) {
                g.setColour(connection->connectionColor);
            } else {
                g.setColour(connection->connectionColor.withAlpha(0.4f));
            }

            g.fillEllipse(connection->startPt.getX() - (pointSize / 2),
                          connection->startPt.getY() - (pointSize / 2),
                          pointSize, pointSize);

            g.strokePath(connection->connectionPath, juce::PathStrokeType(2.f));

            g.fillEllipse(connection->endPt.getX() - (pointSize / 2),
                      connection->endPt.getY() - (pointSize / 2),
                      pointSize,
                      pointSize);
        }
    }
}

void ConnectionOverlay::mouseMove(const juce::MouseEvent& e, juce::Point<float> mousePos, float scrollDeltaY)
{
    if (! e.mods.isCtrlDown()) {
        if (ctrlWasDown) clearHighlights();
        return;
    }

    ctrlWasDown = true;

    if (scrollDeltaY > 0)      ++selection;
    else if (scrollDeltaY < 0) --selection;

    const auto mouseInGlobal = mousePos.roundToInt();
    hoveredJack = nullptr;

    for (auto& c : connections) {
        if (c.startJack->getScreenBounds().contains(mouseInGlobal)) { hoveredJack = c.startJack; break; }
        if (c.endJack->getScreenBounds().contains(mouseInGlobal))   { hoveredJack = c.endJack;   break; }
    }

    if (hoveredJack != nullptr)
        grabKeyboardFocus();

    rebuildHighlights();
}

void ConnectionOverlay::beginConnection(OutJack &outJack, bool isInCanvas) {
    liveConnection.connectionPath.clear();
    isDragging = true;
    liveConnection.isStartInCanvas = isInCanvas;

    if (isInCanvas) {
        const auto jackCenterInGlobal = outJack.localPointToGlobal(outJack.getLocalBounds().getCentre().toFloat());
        liveConnection.startPt = getLocalPoint (nullptr, jackCenterInGlobal);
        liveConnection.startJack = &outJack;
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

void ConnectionOverlay::endConnection() {
    isDragging = false;
    liveConnection.isConnectionShown = false;
    liveConnection.isEndActive = false;
    liveConnection.connectionPath.clear();

    for (const auto& inJack : inJacks) {
        if (inJack->getHovering()) {
            bool repeat = false;
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
                liveConnection.endJack = inJack;
                liveConnection.startJack->incrementNumConnections();
                liveConnection.connectionPath.startNewSubPath(liveConnection.startPt);
                liveConnection.connectionPath.lineTo(liveConnection.endPt);

                highlightedConnections.clear();
                connections.push_back(liveConnection);

                if (connectDSP) {
                    connectDSP(liveConnection.startJack->getID(),
                               liveConnection.startJack->getPort(),
                               inJack->getID(), inJack->getPort());
                }
            }
        }

        inJack->setConnecting(false);
        inJack->setHovering(false);
    }

    repaint();
}

void ConnectionOverlay::removeConnection(const Connection& connectionToRemove, std::vector<Connection>& connectionList) {
    auto it = connectionList.begin();
    for (const auto& connection : connectionList) {
        if (connection.startJack == connectionToRemove.startJack &&
            connection.endJack == connectionToRemove.endJack) {

            connectionList.erase(it);
        }

        ++it;
    }
}

bool ConnectionOverlay::keyPressed (const juce::KeyPress& key)
{
    const auto mods = key.getModifiers();
    const bool isDeleteKey =
        key.getKeyCode() == juce::KeyPress::deleteKey
        || key.getKeyCode() == juce::KeyPress::backspaceKey;

    if (!mods.isCtrlDown() || !isDeleteKey)
        return false;

    const auto selected = std::ranges::find_if(highlightedConnections,
                                               [] (const Connection* c) { return c->isSelected; });

    if (selected == highlightedConnections.end())
        return false;

    // Copy before erasing: selected points into highlightedConnections.
    const Connection* toRemove = *selected;

    if (disconnectDSP) {
        disconnectDSP(toRemove->startJack->getID(),
                      toRemove->startJack->getPort(),
                      toRemove->endJack->getID(),
                      toRemove->endJack->getPort());
    }

    const auto it = std::ranges::find_if(connections,
                                         [&] (const Connection& c)
                                         {
                                             return c.startJack == toRemove->startJack
                                                    && c.endJack == toRemove->endJack;
                                         });

    if (it != connections.end())
    {
        it->startJack->decrementNumConnections();
        it->startJack->repaint();

        connections.erase(it);  // every pointer in highlightedConnections is now stale
        selection = 0;
        rebuildHighlights();    // collect fresh pointers and update the slider
    }

    return true;
}

void ConnectionOverlay::modifierKeysChanged (const juce::ModifierKeys& mods)
{
    if (! mods.isCtrlDown() && ctrlWasDown)
        clearHighlights();
}

void ConnectionOverlay::clearHighlights()
{
    ctrlWasDown = false;
    modulationSlider.setVisible (false);
    highlightedConnections.clear();

    for (auto& c : connections) {
        c.isConnectionShown = false;
        c.isSelected = false;
    }

    repaint();
}

void ConnectionOverlay::rebuildHighlights()
{
    highlightedConnections.clear();

    for (auto& c : connections) {
        c.isConnectionShown = false;
        c.isSelected = false;
    }

    if (hoveredJack != nullptr)
        for (auto& c : connections)
            if (c.startJack == hoveredJack || c.endJack == hoveredJack) {
                c.isConnectionShown = true;
                highlightedConnections.push_back(&c);
            }

    const int n = static_cast<int>(highlightedConnections.size());

    if (n == 0) {
        modulationSlider.setVisible(false);
        repaint();
        return;
    }

    selection = ((selection % n) + n) % n;
    auto* sel = highlightedConnections[selection];
    sel->isSelected = true;

    auto topLeft = sel->startJack->getScreenBounds().getTopLeft();
    if (sel->isStartInCanvas)
        topLeft = getLocalPoint(nullptr, topLeft);

    modulationSlider.setBounds(topLeft.x, topLeft.y,
                               sel->startJack->getWidth(), sel->startJack->getHeight());
    modulationSlider.setValue(sel->modAmount, juce::dontSendNotification);
    modulationSlider.setColour(juce::Slider::rotarySliderFillColourId, colors.getColor(ThemeColors::visuals));
    modulationSlider.setVisible(true);

    repaint();
}
