#include "filter.h"

void FilterModuleComponent::drawBody(juce::Graphics &g) {
    drawFilterVisual(g);
    drawIOSection(g);
}

void FilterModuleComponent::drawIOSection(juce::Graphics &g) {
    filterType.setColour(juce::ComboBox::textColourId, comboBoxTextColor);
    filterSpec.setColour(juce::ComboBox::textColourId, comboBoxTextColor);

    res.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    cut.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    mix.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    blend.setColour(juce::Slider::rotarySliderFillColourId, statusColor);

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

    g.drawText(resLabel.getText(), resLabelBounds, juce::Justification::centred);
    g.drawText(cutLabel.getText(), cutLabelBounds, juce::Justification::centred);
    g.drawText(inJackLabel.getText(), inJackLabelBounds, juce::Justification::centred);
    g.drawText(mixLabel.getText(), mixLabelBounds, juce::Justification::centred);
    g.drawText(blendLabel.getText(), blendLabelBounds, juce::Justification::centred);
}

void FilterModuleComponent::drawFilterVisual(juce::Graphics &g) const {
    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillRoundedRectangle(filterBounds, cornerRounding);

    g.setGradientFill(filterGradient);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (filterWavePathClosed);
        g.fillRoundedRectangle (filterGradientRect, 5);
    }

    g.setColour(visualColor);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (filterBounds.toNearestInt());
        g.strokePath(filterWavePath, juce::PathStrokeType (2.f));
    }
}

void FilterModuleComponent::onPathsReady() {
    headerBounds.removeFromRight(105);
    headerBounds.removeFromLeft(105);

    constexpr auto halfCenterHeight = 12.5f;
    const auto halfHeight = body.getHeight() / 2;
    constexpr auto offset = 5.f;

    filterBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());
    centerBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());
    IOBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());

    filterBounds.removeFromBottom(halfHeight + halfCenterHeight + offset);
    filterBounds.removeFromTop(BGInset);
    filterBounds.removeFromLeft(BGInset);
    filterBounds.removeFromRight(BGInset);
    filterBounds.removeFromBottom(BGInset / 2);

    centerBounds.removeFromTop(halfHeight - halfCenterHeight - offset);
    centerBounds.removeFromBottom(halfHeight - halfCenterHeight + offset);

    IOBounds.removeFromTop(halfHeight + halfCenterHeight - offset);

    filterGradientRect.setBounds(filterBounds.getX(), filterBounds.getY(), filterBounds.getWidth(), filterBounds.getHeight());

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

    connectListener(filterType, FilterDSP::filterTypeIn); // combo box connections
    connectListener(filterSpec, FilterDSP::filterSpecIn);

    connectListener(res, FilterDSP::resIn);         // slider connections
    connectListener(cut, FilterDSP::cutIn);
    connectListener(mix, FilterDSP::mixIn);
    connectListener(blend, FilterDSP::blendIn);

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

    constexpr float nudge = 5.f;
    constexpr float smallJackSize = 20.f;

    const float topY = controlBounds[0].getY();
    const float middleY = controlBounds[0].getCentreY();

    addAndMakeVisible(res);
    addAndMakeVisible(resLabel);
    res.setNormalisableRange (logRange (0.5, 20.0));
    res.setBounds(leftKnobX - (bigRotarySize / 2), lowerKnobY, bigRotarySize, bigRotarySize);
    resLabelBounds.setBounds(leftKnobX - (bigRotarySize / 2),
                         lowerKnobY - IOInsetX + 2,
                         bigRotarySize,
                         10);

    addAndMakeVisible(cut);
    cut.setNormalisableRange (logRange (8.0, 22050.0));
    cut.setBounds(leftCenterKnobX - (bigRotarySize / 2) - nudge, middleKnobY, bigRotarySize, bigRotarySize);
    cutLabelBounds.setBounds(leftCenterKnobX - (bigRotarySize / 2) - nudge,
                         middleKnobY + bigRotarySize - 7,
                         bigRotarySize,
                         15);

    addAndMakeVisible(signalInJack);
    signalInJack.setBounds(controlBounds[0].getCentreX() - (smallJackSize / 2),
                     topY + ((middleY - topY) / 2) - (smallJackSize / 2) - 2,
                    smallJackSize,
                    smallJackSize);
    inJackLabelBounds.setBounds(controlBounds[0].getCentreX() - 25,
                         topY + ((middleY - topY) / 2) - (smallJackSize / 2) + smallJackSize,
                         50,
                         15);

    addAndMakeVisible(mix);
    mix.setBounds(rightCenterKnobX - (bigRotarySize / 2) + nudge, middleKnobY, bigRotarySize, bigRotarySize);
    mixLabelBounds.setBounds(rightCenterKnobX - (bigRotarySize / 2) + nudge,
                         middleKnobY + bigRotarySize - 7,
                         bigRotarySize,
                         15);

    addAndMakeVisible(blend);
    blend.setBounds(rightKnobX - (bigRotarySize / 2), lowerKnobY, bigRotarySize, bigRotarySize);
    blendLabelBounds.setBounds(rightKnobX - (bigRotarySize / 2),
                         lowerKnobY - IOInsetX + 2,
                         bigRotarySize,
                         10);

    addAndMakeVisible(outJack);
    outJack.setBounds(body.getCentreX() - (jackSize / 2),
                         body.getBottom() - jackSize - jackBorderSize,
                         jackSize, jackSize);

    jackBorder = outJack.getBounds().toFloat().expanded(jackBorderSize, jackBorderSize);
}

void FilterModuleComponent::connectListener(juce::Slider &slider, int control) {
    slider.onValueChange = [this, &slider, control] {
        if (onParameterChanged)
            onParameterChanged (getModuleID(), control, static_cast<float> (slider.getValue()));
        setFilterPath();
        repaint();
    };
}

void FilterModuleComponent::connectListener(juce::ComboBox &comboBox, int control) {
    comboBox.onChange = [this, &comboBox, control] {
        if (onParameterChanged)
            onParameterChanged (getModuleID(), control, static_cast<float> (comboBox.getSelectedId()));
        setFilterPath();
        repaint();
    };
}

void FilterModuleComponent::setVisualGradient() {
    state ?
    filterGradient = juce::ColourGradient::vertical(visualColor.withMultipliedAlpha(0.4),
                                               colors.getColor(ThemeColors::canvasBG).withMultipliedAlpha(0.4),
                                                          filterGradientRect) :
    filterGradient = juce::ColourGradient::vertical(visualColor,
                                               colors.getColor(ThemeColors::canvasBG),
                                                          filterGradientRect);
}

void FilterModuleComponent::setFilterPath() {
    filterWavePath.clear();
    filterWavePathClosed.clear();

    const float left = filterBounds.getX(),  right  = filterBounds.getRight();
    const float top = filterBounds.getY(),  bottom = filterBounds.getBottom();
    const float width = filterBounds.getWidth();

    constexpr float minDb = -24.f, maxDb = 24.f;
    constexpr float minFreq = 20.f, maxFreq = 20000.f;
    constexpr float fs = 48000.f;                     // ideally the real sample rate
    constexpr float pi = juce::MathConstants<float>::pi;

    const auto resonance = std::max (0.01f, static_cast<float> (res.getValue()));
    const auto cutoff = juce::jlimit (8.0f, 0.49f * fs, static_cast<float> (cut.getValue()));

    const float gc = std::tan (pi * cutoff / fs);

    filterWavePathClosed.startNewSubPath (left, bottom);

    const int numPoints = juce::roundToInt (width);
    for (int i = 0; i <= numPoints; ++i)
    {
        const float norm = static_cast<float> (i) / static_cast<float> (numPoints);
        const float freq = minFreq * std::pow (maxFreq / minFreq, norm);

        const float w = std::tan (pi * freq / fs) / gc;
        const float a = 1.0f - w * w;
        const float b = w / resonance;
        const float mag = 1.0f / std::sqrt (a * a + b * b);

        const float db = juce::jlimit (minDb, maxDb, juce::Decibels::gainToDecibels (mag, minDb));
        const float px = left + norm * width;
        const float py = juce::jmap (db, minDb, maxDb, bottom, top);

        if (i == 0) filterWavePath.startNewSubPath (px, py);
        else filterWavePath.lineTo (px, py);

        filterWavePathClosed.lineTo (px, py);
    }

    filterWavePathClosed.lineTo (right, bottom);
    filterWavePathClosed.closeSubPath();
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

juce::NormalisableRange<double> FilterModuleComponent::logRange(double min, double max) {
    return { min, max,
             [] (double start, double end, double norm)  { return start * std::pow (end / start, norm); },
             [] (double start, double end, double value) { return std::log (value / start) / std::log (end / start); } };
}
