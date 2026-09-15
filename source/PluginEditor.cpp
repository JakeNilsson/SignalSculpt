#include "headers/PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible(canvas);
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
    canvas.setBounds(canvasBounds);

    const auto headerBounds = setInversePos(bounds, 0, 0, 0, 970);
    header.setBounds(headerBounds);
    header.setPath();

    const auto footerBounds = setInversePos(bounds, 0, 0, 970, 0);
    footer.setBounds(footerBounds);
    footer.setPath();

    const auto inputsTabBounds = setInversePos(bounds, 0, 1800, 0, 0);
    inputsTab.setBounds(inputsTabBounds);
    inputsTab.setPath();

    const auto outputsTabBounds = setInversePos(bounds, 1800, 0, 0, 0);
    outputsTab.setBounds(outputsTabBounds);
    outputsTab.setPath();
}
