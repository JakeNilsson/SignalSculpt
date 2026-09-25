#include "wave.h"
#include <ranges>

/*void WaveModule::process(juce::AudioBuffer<float> &buffer) {

}

juce::String WaveModule::getName() const {
    return name;
}*/

void WaveModuleComponent::paint(juce::Graphics &g) {
    const auto mousePos = getMouseXYRelative();
    drawStateButton(g, mousePos.toFloat());

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillPath(headerPath);

    g.setColour (statusColor);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (headerPath);
        g.strokePath (headerSinPath, juce::PathStrokeType (2.f));
    }

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(headerBounds);

    g.setColour(statusColor);
    g.setFont(moduleFont);
    g.drawText(name.getText(), headerBounds, juce::Justification::centred);

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(body);

    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillRoundedRectangle(oscBounds, 5);

    if (prevState != state) {
        setVisualGradient();

        state ?
        comboBoxTextColor = colors.getColor(ThemeColors::neutral) :
        comboBoxTextColor = colors.getColor(ThemeColors::tabBG);

        prevState = state;
    }

    shapeType.setColour(juce::ComboBox::textColourId, comboBoxTextColor);
    shapeSpec.setColour(juce::ComboBox::textColourId, comboBoxTextColor);

    phase.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    freq.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    mix.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    det.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    voices.setColour(juce::Slider::rotarySliderFillColourId, statusColor);

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

    g.setColour(statusColor);
    g.setFont(moduleFont.withPointHeight(16.f));
    g.drawText(shapeLabel.getText(), centerBounds, juce::Justification::centredLeft);

    g.setColour(statusColor);
    g.strokePath(IOPath, juce::PathStrokeType(1.f));

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(IOTextBounds);

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillEllipse(jackBorder);

    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillEllipse(jackBounds);

    g.setColour(statusColor);
    //g.setFont(moduleFont.withPointHeight(16.f));
    g.drawText(io.getText(), IOBounds, juce::Justification::centredTop);
}

void WaveModuleComponent::onPathsReady() {
    setHeaderSinPath();

    headerBounds.removeFromRight(105);
    headerBounds.removeFromLeft(105);

    auto halfCenterHeight = 12.5f;
    auto halfHeight = body.getHeight() / 2;
    auto offset = 5.f;
    auto inset = 10.f;
    auto IOinset = 7.5;

    oscBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());
    centerBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());
    IOBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());

    oscBounds.removeFromBottom(halfHeight + halfCenterHeight + offset);
    oscBounds.removeFromTop(inset);
    oscBounds.removeFromLeft(inset);
    oscBounds.removeFromRight(inset);
    oscBounds.removeFromBottom(inset / 2);

    centerBounds.removeFromTop(halfHeight - halfCenterHeight - offset);
    centerBounds.removeFromBottom(halfHeight - halfCenterHeight + offset);

    IOBounds.removeFromTop(halfHeight + halfCenterHeight - offset);

    oscGradientRect.setBounds(oscBounds.getX(), oscBounds.getY(), oscBounds.getWidth(), oscBounds.getHeight());

    centerBounds.reduce(inset, 2);

    addAndMakeVisible(shapeType);
    addAndMakeVisible(shapeSpec);
    auto leftOffset = 55;
    auto halfWidth = (centerBounds.getWidth() - leftOffset) / 2;

    comboBoxTextColor = colors.getColor(ThemeColors::neutral);

    shapeType.setBounds(centerBounds.getX() + leftOffset,
                        centerBounds.getY(),
                        halfWidth,
                        centerBounds.getHeight());

    shapeType.addItemList({"Sine"}, 1);
    shapeType.setSelectedId(1);

    shapeSpec.setBounds(centerBounds.getX() + leftOffset + halfWidth + 7,
                        centerBounds.getY(),
                        halfWidth - 7,
                        centerBounds.getHeight());

    shapeSpec.addItemList({"Analog"}, 1);
    shapeSpec.setSelectedId(1);

    juce::Rectangle<float> IOBoundsReduced = IOBounds.reduced(inset, 0);
    IOBoundsReduced.removeFromBottom(inset);
    IOBoundsReduced.removeFromTop(10);

    IOPath.startNewSubPath(IOBoundsReduced.getTopLeft());
    IOPath.lineTo(IOBoundsReduced.getBottomLeft());
    IOPath.lineTo(IOBoundsReduced.getBottomRight());
    IOPath.lineTo(IOBoundsReduced.getTopRight());
    IOPath.closeSubPath();

    IOTextBounds.setBounds(IOBounds.getX(), IOBounds.getY(), IOBounds.getWidth(), IOBounds.getHeight());
    IOTextBounds.removeFromBottom(IOTextBounds.getHeight() - 20);
    IOTextBounds.removeFromRight(85);
    IOTextBounds.removeFromLeft(85);

    constexpr auto jackSize = 45.f;
    constexpr auto jackBorderSz = 8.f;
    constexpr auto bigRotarySize = 55.f;
    constexpr auto smallRotarySize = 42.5f;

    jackBounds.setBounds(body.getCentreX() - (jackSize / 2),
                         body.getBottom() - jackSize - jackBorderSz,
                         jackSize, jackSize);

    jackBorder = jackBounds.expanded(jackBorderSz, jackBorderSz);

    addAndMakeVisible(phase);
    phase.setRotaryParameters(0, juce::MathConstants<float>::twoPi, false);
    phase.setBounds(
        IOBoundsReduced.getX() + IOinset,
        IOBoundsReduced.getBottom() - bigRotarySize - IOinset,
        bigRotarySize,
        bigRotarySize
    );

    const auto leftMidX = IOBoundsReduced.getX() + (IOBoundsReduced.getWidth() / 4);
    addAndMakeVisible(freq);
    freq.setSkewFactor(2.f);
    freq.setBounds(
        leftMidX - (inset * 0.9),
        IOBoundsReduced.getY() + (IOinset * 1.5),
        bigRotarySize,
        bigRotarySize
    );

    const auto MidX = IOBoundsReduced.getX() + (IOBoundsReduced.getWidth() / 2);
    addAndMakeVisible(mix);
    mix.setBounds(
        MidX - (smallRotarySize / 2),
        IOBoundsReduced.getY() + (IOinset),
        smallRotarySize,
        smallRotarySize
    );

    const auto rightMidX = IOBoundsReduced.getX() + ((3 * IOBoundsReduced.getWidth()) / 4);
    addAndMakeVisible(det);
    det.setBounds(
        rightMidX - inset - (bigRotarySize / 1.45),
        IOBoundsReduced.getY() + (IOinset * 1.5),
        bigRotarySize,
        bigRotarySize
    );

    addAndMakeVisible(voices);
    voices.setBounds(
        IOBoundsReduced.getRight() - bigRotarySize - IOinset,
        IOBoundsReduced.getBottom() - bigRotarySize - IOinset,
        bigRotarySize,
        bigRotarySize
    );

    setVisualGradient();

    setOscPath();
}

void WaveModuleComponent::setHeaderSinPath() {
    const auto startPt = headerBounds.getX() + (headerBounds.getHeight() / 2) + 2;
    const auto amplitude = 3;

    headerSinPath.startNewSubPath(body.getX(), startPt);
    for (auto x = static_cast<int>(body.getX()); x < static_cast<int>(body.getWidth()) + body.getX(); x++) {
        headerSinPath.lineTo(static_cast<float>(x), amplitude * std::sin(0.25 * x) + startPt);
    }
}

void WaveModuleComponent::setOscPath() {
    const juce::Point startPt = {oscBounds.getX(), oscBounds.getCentreY()};
    const juce::Point endPt = {oscBounds.getRight(), oscBounds.getCentreY()};
    const auto amplitude = 35.f;
    const auto width = oscBounds.getWidth();

    oscWavePath.clear();
    oscWavePathClosed.clear();

    oscWavePath.startNewSubPath(startPt);
    oscWavePathClosed.startNewSubPath(startPt);

    for (auto x = startPt.getX() - 10; x < oscBounds.getRight() + 10; x++) {
        const auto t = (x - startPt.getX()) / width;
        const auto y = amplitude * std::sin(t * juce::MathConstants<float>::twoPi) + startPt.getY();

        oscWavePath.lineTo(x, y);
        oscWavePathClosed.lineTo(x, y);
    }

    oscWavePathClosed.lineTo(endPt);
    oscWavePathClosed.closeSubPath();
}

void WaveModuleComponent::setVisualGradient() {
    state ?
    oscGradient = juce::ColourGradient::vertical(visualColor.withMultipliedAlpha(0.4),
                                               visualColor.withMultipliedAlpha(0.4),
                                                          oscGradientRect) :
    oscGradient = juce::ColourGradient::vertical(visualColor,
                                               visualColor,
                                                          oscGradientRect);

    oscGradient.addColour(0.5f, juce::Colour(colors.getColor(ThemeColors::canvasBG)));
}
