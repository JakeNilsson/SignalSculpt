#pragma once

#include "Canvas.h"
#include "PluginProcessor.h"
#include "Header.h"
#include "Footer.h"
#include "InputsTab.h"
#include "OutputsTab.h"
#include "theme/ConnectionOverlay.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void resized() override;

    static juce::Rectangle<int> setInversePos(juce::Rectangle<int> parentDimensions,
                                              int left,
                                              int right,
                                              int top,
                                              int bottom);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;

    Colors colors;
    SiScLookAndFeel siscLookAndFeel{colors};

    Header header{colors};
    Footer footer{colors};
    InputsTab inputsTab{colors};
    OutputsTab outputsTab{colors};
    Canvas canvas{colors, canvasContainer, *this, connectionOverlay};
    juce::Viewport canvasContainer;

    ConnectionOverlay connectionOverlay{colors, canvasContainer};

    juce::OwnedArray<juce::Path> connections;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
