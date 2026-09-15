#include "PluginEditor.h"

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
    setSize (1720, 880);
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
    const auto canvasBounds = getBounds();
    canvas.setBounds(canvasBounds);

    const auto headerBounds = setInversePos(getBounds(), 0, 0, 0, 780);
    header.setBounds(headerBounds);

    const auto footerBounds = setInversePos(getBounds(), 0, 0, 780, 0);
    footer.setBounds(footerBounds);

    const auto inputsTabBounds = setInversePos(getBounds(), 0, 1640, 0, 0);
    inputsTab.setBounds(inputsTabBounds);

    const auto outputsTabBounds = setInversePos(getBounds(), 1640, 0, 0, 0);
    outputsTab.setBounds(outputsTabBounds);
}
