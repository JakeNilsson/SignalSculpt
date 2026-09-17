#pragma once
#include <cmath>

#include "Colors.h"

class Canvas : public juce::Component {
public:
    explicit Canvas (Colors& colorsRef) : colors (colorsRef) {}

    void mouseMove (const juce::MouseEvent& e) override
    {
        auto pos = e.position; // already relative to this component
        float blockX = std::floor (pos.x / gridSizePx) * gridSizePx;
        float blockY = std::floor (pos.y / gridSizePx) * gridSizePx;
        juce::Rectangle<float> block (blockX, blockY, gridSizePx, gridSizePx);
        block.reduce(10.f, 10.f);

        if (block != lastMouseBlock)
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

        for (float x = firstX; x < clipRight; x += gridSizePx) {
            gridLines.setStart(x, top + startGridOffset);
            gridLines.setEnd(x, bottom);
            g.drawDashedLine (gridLines, dashLengths, 2, 2.f, 0);
        }

        for (float y = firstY; y < clipBottom; y += gridSizePx) {
            gridLines.setStart(left + startGridOffset, y);
            gridLines.setEnd(right, y);
            g.drawDashedLine (gridLines, dashLengths, 2, 2.f, 0);
        }

        g.setColour(colors.getColor (ThemeColors::tabBG).withAlpha(0.25f));
        g.fillRoundedRectangle(lastMouseBlock, cornerRounding);

        g.setColour(colors.getColor (ThemeColors::tabBG));
        g.drawLine (plusVert, 4);
        g.drawLine (plusHoriz, 4);
    }

private:
    float gridSizePx = 160.f;
    float startGridOffset = 15.f;
    float cornerRounding = 5.f;
    static constexpr float dashLengths[] = {10.f, 10.f};

    Colors& colors;

    float plusOffset = 40.f;
    juce::Rectangle<float> lastMouseBlock;
    juce::Line<float> plusVert;
    juce::Line<float> plusHoriz;
};