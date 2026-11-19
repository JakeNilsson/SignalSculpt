#pragma once
#include <cmath>
#include "theme/Colors.h"
#include "helpers/moduleHandler.h"
#include "theme/ConnectionOverlay.h"
#include "PluginProcessor.h"

class Canvas : public juce::Component {
public:
    explicit Canvas (Colors& colorsRef,
        juce::Viewport& canvasContainerRef,
        ConnectionOverlay& overlayRef) : colors(colorsRef),
                                         moduleHandler(colors, moduleComponents),
                                         canvasContainer(canvasContainerRef),
                                         overlay(overlayRef) {}

    void mouseMove (const juce::MouseEvent &e) override
    {
        if (const auto block = blockAt (e.position).reduced (10.f); block != lastMouseBlock)
        {
            repaint (lastMouseBlock.getSmallestIntegerContainer());
            repaint (block.getSmallestIntegerContainer());
            lastMouseBlock = block;

            plusVert.setStart(block.getCentreX(), block.getY() + plusOffset);
            plusVert.setEnd(block.getCentreX(), block.getBottom() - plusOffset);

            plusHoriz.setStart(block.getX() + plusOffset, block.getCentreY());
            plusHoriz.setEnd(block.getRight() - plusOffset, block.getCentreY());
        }
    }

    void mouseExit (const juce::MouseEvent&) override
    {
        repaint (lastMouseBlock.getSmallestIntegerContainer());
        lastMouseBlock = {};
        plusVert = {};
        plusHoriz = {};
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        pressStartedHere = true;
        mouseMove (e);   // refresh the hover square
        mouseIsDown = true;
        repaint (lastMouseBlock.getSmallestIntegerContainer());
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        if (pressStartedHere && ! mouseDragged)
            showModuleMenu (blockAt (e.position));

        pressStartedHere = false;
        mouseDragged = false;
        mouseIsDown = false;
        repaint (lastMouseBlock.getSmallestIntegerContainer());
    }

    void mouseDrag(const juce::MouseEvent&) override {
        mouseDragged = true;
    }

    void paint (juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        auto clip = g.getClipBounds().toFloat();

        g.setColour (colors.getColor (ThemeColors::canvasBG));
        g.fillRect (bounds);

        g.setColour (colors.getColor (ThemeColors::tabBG));

        auto left = bounds.getX();
        auto right = bounds.getRight();
        auto top = bounds.getY();
        auto bottom = bounds.getBottom();

        auto clipLeft = clip.getX();
        auto clipRight = clip.getRight();
        auto clipTop = clip.getY();
        auto clipBottom = clip.getBottom();

        const float firstX = std::ceil(clipLeft / gridSizePx) * gridSizePx;
        const float firstY = std::ceil(clipTop / gridSizePx) * gridSizePx;

        juce::Line<float> gridLines;

        for (int i = 0; ; ++i) {
            const float x = firstX + static_cast<float>(i) * gridSizePx;
            if (!(x < clipRight))
                break;

            gridLines.setStart(x, top + startGridOffset);
            gridLines.setEnd(x, bottom);
            g.drawDashedLine(gridLines, dashLengths, 2, 2.f, 0);
        }

        for (int i = 0; ; ++i) {
            const float y = firstY + static_cast<float>(i) * gridSizePx;
            if (!(y < clipBottom))
                break;

            gridLines.setStart(left + startGridOffset, y);
            gridLines.setEnd(right, y);
            g.drawDashedLine(gridLines, dashLengths, 2, 2.f, 0);
        }

        g.setColour(mouseIsDown ? colors.getColor(ThemeColors::neutral).withAlpha(0.3f) : colors.getColor(ThemeColors::tabBG).withAlpha(0.25f));
        g.fillRoundedRectangle(lastMouseBlock, cornerRounding);

        g.setColour(mouseIsDown ? colors.getColor(ThemeColors::canvasBG) : colors.getColor(ThemeColors::tabBG));
        g.drawLine (plusVert, 4);
        g.drawLine (plusHoriz, 4);
    }

    void showModuleMenu(const juce::Rectangle<float> targetBlock)
    {
        juce::PopupMenu menu;
        menu.addItem (juce::toUnderlyingType (ModuleType::Wave),     "Wave");
        menu.addItem (juce::toUnderlyingType (ModuleType::Envelope), "Envelope");
        menu.addItem (juce::toUnderlyingType (ModuleType::Filter),   "Filter");

        menu.showMenuAsync (juce::PopupMenu::Options()
                        .withTargetComponent (this)
                        .withMousePosition(),
                        [safeThis = juce::Component::SafePointer<Canvas> (this), targetBlock] (const int result)
        {
            if (safeThis != nullptr && result != 0)
                safeThis->addModuleAt (static_cast<ModuleType> (result), targetBlock);
        });
    }

    void addModuleAt (const ModuleType moduleType, juce::Rectangle<float> block)
    {
        if (moduleType == ModuleType::None)
            return;

        moduleHandler.addModuleComponent (moduleType);
        const auto newChild = moduleComponents.getLast();
        addAndMakeVisible (newChild);

        if (onCreateDspModule)
            newChild->setModuleID (onCreateDspModule (moduleType));

        block.setWidth  (block.getWidth()  * newChild->getBlockSizeX());
        block.setHeight (block.getHeight() * newChild->getBlockSizeY());

        newChild->setBounds (block.toNearestInt());
        newChild->setPaths();
        newChild->repaint();

        overlay.addInJacks(newChild->getInJacks());

        newChild->onDelete = [this] (const int moduleID) {
            moduleHandler.removeModuleComponent(moduleID, overlay);
        };

        newChild->onParameterChanged = [this] (const int moduleID, const int paramID, const float value) {
            if (onSetDspParameter) onSetDspParameter(moduleID, paramID, value);
        };

        newChild->getOutJack().onDragStart = [this] (OutJack& outJack, const juce::MouseEvent&) {
            overlay.beginConnection (outJack, true);
        };

        newChild->getOutJack().onDrag = [this] (const juce::Point<float> mousePos) {
            overlay.updateConnection (mousePos);
        };

        newChild->getOutJack().onDragEnd = [this] (const juce::MouseEvent&) {
            overlay.endConnection ();
        };
    }

    [[nodiscard]] juce::Rectangle<float> blockAt (const juce::Point<float> pos) const
    {
        const float x = std::floor (pos.x / gridSizePx) * gridSizePx;
        const float y = std::floor (pos.y / gridSizePx) * gridSizePx;
        return { x, y, gridSizePx, gridSizePx };
    }

    std::function<int (ModuleType)> onCreateDspModule;   // returns the new module's ID
    std::function<void (int)>       onRemoveDspModule;

    std::function<void (int, int, float)> onSetDspParameter;

private:
    float gridSizePx = 160.f;
    float startGridOffset = 15.f;
    float cornerRounding = 5.f;
    static constexpr float dashLengths[] = {10.f, 10.f};

    bool mouseIsDown = false;
    bool mouseDragged = false;
    bool pressStartedHere = false;

    Colors& colors;

    float plusOffset = 40.f;
    juce::Rectangle<float> lastMouseBlock;
    juce::Line<float> plusVert;
    juce::Line<float> plusHoriz;

    juce::OwnedArray<ModuleComponent> moduleComponents;
    ModuleHandler moduleHandler;

    juce::Viewport& canvasContainer;
    ConnectionOverlay& overlay;

    juce::OwnedArray<juce::Path> connections;
};