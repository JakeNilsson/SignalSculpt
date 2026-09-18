#pragma once
#include "Canvas.h"
#include "SiScLookAndFeel.h"

class CanvasContainer : public juce::Viewport {
public:
    explicit CanvasContainer(Canvas &canvasRef) : canvas(canvasRef) {};

private:
    juce::Viewport viewport;

    Canvas &canvas;

    float width,
          height,
          gridSize;
};
