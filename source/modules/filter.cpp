#include "filter.h"

void FilterModuleComponent::drawBody(juce::Graphics &g) {
    drawFilterVisual(g);
    drawIOSection(g);
}

void FilterModuleComponent::drawIOSection(juce::Graphics &g) {
    filterType.setColour(juce::ComboBox::textColourId, comboBoxTextColor);
    filterSpec.setColour(juce::ComboBox::textColourId, comboBoxTextColor);

    g.setColour(statusColor);
    g.setFont(moduleFont.withPointHeight(16.f));
    g.drawText(filterLabel.getText(), centerBounds, juce::Justification::centredLeft);

    g.setColour(statusColor);
    g.strokePath(IOPath, juce::PathStrokeType(1.f));

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(IOTextBounds);

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillEllipse(jackBorder);

    g.setColour(statusColor);
    g.drawText(io.getText(), IOBounds, juce::Justification::centredTop);

    g.drawText(inJackLabel.getText(), inJackLabelBounds, juce::Justification::centred);
}

void FilterModuleComponent::drawFilterVisual(juce::Graphics &g) const {
    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillRoundedRectangle(oscBounds, cornerRounding);

    g.setGradientFill(oscGradient);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (oscWavePathClosed);
        g.fillRoundedRectangle (oscGradientRect, 5);
    }

    g.setColour(visualColor);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (oscBounds.toNearestInt());
        g.strokePath(oscWavePath, juce::PathStrokeType (2.f));
    }
}

void FilterModuleComponent::onPathsReady() {
    headerBounds.removeFromRight(105);
    headerBounds.removeFromLeft(105);

    constexpr auto halfCenterHeight = 12.5f;
    const auto halfHeight = body.getHeight() / 2;
    constexpr auto offset = 5.f;

    oscBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());
    centerBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());
    IOBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());

    oscBounds.removeFromBottom(halfHeight + halfCenterHeight + offset);
    oscBounds.removeFromTop(BGInset);
    oscBounds.removeFromLeft(BGInset);
    oscBounds.removeFromRight(BGInset);
    oscBounds.removeFromBottom(BGInset / 2);

    centerBounds.removeFromTop(halfHeight - halfCenterHeight - offset);
    centerBounds.removeFromBottom(halfHeight - halfCenterHeight + offset);

    IOBounds.removeFromTop(halfHeight + halfCenterHeight - offset);

    oscGradientRect.setBounds(oscBounds.getX(), oscBounds.getY(), oscBounds.getWidth(), oscBounds.getHeight());

    centerBounds.reduce(BGInset, 2);

    addAndMakeVisible(filterType);
    addAndMakeVisible(filterSpec);
    auto leftOffset = 55;
    auto halfWidth = (centerBounds.getWidth() - leftOffset) / 2;

    filterType.setBounds(centerBounds.getX() + leftOffset,
                        centerBounds.getY(),
                        halfWidth,
                        centerBounds.getHeight());

    filterType.addItemList({"Analog"}, 1);
    filterType.setSelectedId(1);

    filterSpec.setBounds(centerBounds.getX() + leftOffset + halfWidth + 7,
                        centerBounds.getY(),
                        halfWidth - 7,
                        centerBounds.getHeight());

    filterSpec.addItemList({"12 dB", "24 dB"}, 1);
    filterSpec.setSelectedId(1);

    juce::Rectangle<float> IOBoundsReduced = IOBounds.reduced(BGInset, 0);
    IOBoundsReduced.removeFromBottom(BGInset);
    IOBoundsReduced.removeFromTop(BGInset);

    IOPath.startNewSubPath(IOBoundsReduced.getTopLeft());
    IOPath.lineTo(IOBoundsReduced.getBottomLeft());
    IOPath.lineTo(IOBoundsReduced.getBottomRight());
    IOPath.lineTo(IOBoundsReduced.getTopRight());
    IOPath.closeSubPath();

    IOTextBounds.setBounds(IOBounds.getX(), IOBounds.getY(), IOBounds.getWidth(), IOBounds.getHeight());
    IOTextBounds.removeFromBottom(IOTextBounds.getHeight() - 20);
    IOTextBounds.removeFromRight(85);
    IOTextBounds.removeFromLeft(85);

    /*connectListener(shapeType, WaveDSP::shapeTypeIn); // combo box connections

    connectListener(phase, WaveDSP::phaseIn);         // slider connections
    connectListener(freq, WaveDSP::freqIn);
    connectListener(mix, WaveDSP::mixIn);
    connectListener(det, WaveDSP::detIn);
    connectListener(voices, WaveDSP::voicesIn);*/

    setControlBounds({IOBoundsReduced});
    setVisualGradient();
    setFilterPath();
}

void FilterModuleComponent::setControlBounds(const std::vector<juce::Rectangle<float> > &controlBounds) {
    const auto leftKnobX = controlBounds[0].getX() + IOInsetX + (bigRotarySize / 2);
    const auto centerKnobX = controlBounds[0].getCentreX();
    const auto rightKnobX = controlBounds[0].getRight() - IOInsetX - (bigRotarySize / 2);

    const auto leftCenterKnobX = ((centerKnobX - leftKnobX) / 2) + leftKnobX + (IOInsetX / 3);
    const auto rightCenterKnobX = ((rightKnobX - centerKnobX) / 2) + centerKnobX - (IOInsetX / 3);

    const auto lowerKnobY = controlBounds[0].getBottom() - bigRotarySize - IOInsetY;
    const auto middleKnobY = controlBounds[0].getY() + (IOInsetY * 1.4);
    const auto upperKnobY = controlBounds[0].getY() + (IOInsetY * 0.5);

    constexpr float smallJackSize = 20.f;

    const float topY = controlBounds[0].getY();
    const float middleY = controlBounds[0].getCentreY();

    addAndMakeVisible(signalInJack);
    signalInJack.setBounds(controlBounds[0].getCentreX() - (smallJackSize / 2),
                     topY + ((middleY - topY) / 2) - (smallJackSize / 2),
                    smallJackSize,
                    smallJackSize);
    inJackLabelBounds.setBounds(controlBounds[0].getCentreX() - 25,
                         topY + ((middleY - topY) / 2) - (smallJackSize / 2) + smallJackSize,
                         50,
                         15);

    addAndMakeVisible(outJack);
    outJack.setBounds(body.getCentreX() - (jackSize / 2),
                         body.getBottom() - jackSize - jackBorderSize,
                         jackSize, jackSize);

    jackBorder = outJack.getBounds().toFloat().expanded(jackBorderSize, jackBorderSize);
}

void FilterModuleComponent::connectListener(juce::Slider &slider, int control) const {
    slider.onValueChange = [this, &slider, control] {
        if (onParameterChanged)
            onParameterChanged (getModuleID(), control, static_cast<float> (slider.getValue()));
    };
}

void FilterModuleComponent::connectListener(juce::ComboBox &comboBox, int control) const {
    comboBox.onChange = [this, &comboBox, control] {
        if (onParameterChanged)
            onParameterChanged (getModuleID(), control, static_cast<float> (comboBox.getSelectedId()));
    };
}

void FilterModuleComponent::setVisualGradient() {

}

void FilterModuleComponent::setFilterPath() {

}

void FilterModuleComponent::setHeaderDesignPath() {
    const auto startPt = headerBounds.getX() + (headerBounds.getHeight() / 2);

    headerDesignPath.clear();

    headerDesignPath.startNewSubPath(body.getX(), startPt);
    headerDesignPath.lineTo(body.getRight(), startPt);
}

void FilterModuleComponent::setJackIDs(const int ID) {
    signalInJack.setID (ID);
    filterType.getInJack().setID (ID);
    filterSpec.getInJack().setID (ID);

    outJack.setID (ID);
}

juce::Array<InJack *> FilterModuleComponent::getInJacks() {
    juce::Array<InJack *> inJacks;

    inJacks.add(&signalInJack);
    inJacks.add(&filterType.getInJack());
    inJacks.add(&filterSpec.getInJack());

    return inJacks;
}