#include "headers/PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible(canvasContainer);
    canvasContainer.setViewedComponent(&canvas, false);

    addAndMakeVisible(inputsTab);
    addAndMakeVisible(outputsTab);
    addAndMakeVisible(header);
    addAndMakeVisible(footer);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1920, 1080);
}

PluginEditor::~PluginEditor()
{
}

juce::Rectangle<int> PluginEditor::setInversePos(juce::Rectangle<int> parentDimensions,
                                                  const int left,
                                                  const int right,
                                                  const int top,
                                                  const int bottom) {
    auto bounds = parentDimensions;

    bounds.removeFromLeft(left);
    bounds.removeFromRight(right);
    bounds.removeFromTop(top);
    bounds.removeFromBottom(bottom);

    return bounds;
}

void PluginEditor::resized()
{
    const auto bounds = getLocalBounds();

    const auto canvasBounds = bounds;
    //canvasContainer.setBounds(canvasBounds);
    //canvasContainer.setCanvasBounds(canvasBounds);
    canvasContainer.setBounds (canvasBounds);
    canvas.setBounds (0, 0,
                      canvasBounds.getWidth() * 2,
                      canvasBounds.getHeight() * 2);

    const auto headerBounds = setInversePos(bounds, 0, 0, 0, 860);
    header.setBounds(headerBounds);
    header.setTabBounds();

    const auto footerBounds = setInversePos(bounds, 0, 0, 860, 0);
    footer.setBounds(footerBounds);
    footer.setTabBounds();

    const auto inputsTabBounds = setInversePos(bounds, 0, 1800, 110, 0);
    inputsTab.setBounds(inputsTabBounds);
    inputsTab.setTabBounds();

    const auto outputsTabBounds = setInversePos(bounds, 1800, 0, 0, 110);
    outputsTab.setBounds(outputsTabBounds);
    outputsTab.setTabBounds();
}
