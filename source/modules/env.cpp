#include "env.h"

void EnvModuleComponent::drawBody(juce::Graphics &g) {
    drawEnvVisual(g);
    drawIOSection(g);
}

void EnvModuleComponent::drawEnvVisual(juce::Graphics &g) const {
    g.setColour(colors.getColor(ThemeColors::canvasBG));
    g.fillRoundedRectangle(envBounds, 5); // draws envelope rounded rect BG

    g.setGradientFill(envGradient);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (envPathClip);
        g.reduceClipRegion (envPathClosed);
        g.fillRoundedRectangle (envGradientRect, 5); // clips gradient to the closed env path
    }

    g.setColour(visualColor);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (envPathClip);
        g.strokePath(envPath, juce::PathStrokeType (2.f)); // clips path itself to the env BG
    }
}

void EnvModuleComponent::drawIOSection(juce::Graphics &g) {
    delay.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    attack.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    hold.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    decay.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    sustain.setColour(juce::Slider::rotarySliderFillColourId, statusColor);
    release.setColour(juce::Slider::rotarySliderFillColourId, statusColor);

    g.setColour(statusColor);
    g.strokePath(IOPath, juce::PathStrokeType(1.f)); // outline

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(IOTextBounds); // BG for IO Text

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillEllipse(jackBorder); // BG for OutJack

    g.setColour(statusColor);
    g.setFont(moduleFont.withPointHeight(16.f));
    g.drawText(io.getText(), IOBounds, juce::Justification::centredTop); // IO Text

    g.setColour(statusColor);
    g.setFont(moduleFont.withPointHeight(16.f));

    g.drawText(inJackLabel.getText(), inJackLabelBounds, juce::Justification::centred); // GATE Input Text
    g.drawText(delayLabel.getText(), delayLabelBounds, juce::Justification::centred); // Delay Text
    g.drawText(attackLabel.getText(), attackLabelBounds, juce::Justification::centred); // Attack Text
    g.drawText(holdLabel.getText(), holdLabelBounds, juce::Justification::centred); // Hold Text
    g.drawText(decayLabel.getText(), decayLabelBounds, juce::Justification::centred); // Decay Text
    g.drawText(sustainLabel.getText(), sustainLabelBounds, juce::Justification::centred); // Sustain Text
    g.drawText(releaseLabel.getText(), releaseLabelBounds, juce::Justification::centred); // Release Text
}

void EnvModuleComponent::onPathsReady() {
    headerBounds.removeFromRight(77.5);
    headerBounds.removeFromLeft(77.5);

    const auto halfHeight = body.getHeight() / 2;
    constexpr auto offset = 20.f;

    envBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());
    IOBounds.setBounds(body.getX(), body.getY(), body.getWidth(), body.getHeight());

    envBounds.removeFromTop(BGInset);
    envBounds.removeFromLeft(BGInset);
    envBounds.removeFromRight(BGInset);
    envBounds.removeFromBottom((BGInset / 2) + (halfHeight + offset));

    envPathClip.addRoundedRectangle(envBounds, cornerRounding);

    innerEnvBounds = envBounds.reduced(0, 1);

    envGradientRect.setBounds(innerEnvBounds.getX(), innerEnvBounds.getY(), innerEnvBounds.getWidth(), innerEnvBounds.getHeight());

    IOBounds.removeFromTop(halfHeight - offset);

    juce::Rectangle<float> IOBoundsReduced = IOBounds.reduced(BGInset, 0);
    IOBoundsReduced.removeFromBottom(BGInset);
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
    constexpr auto smallJackSize = 20.f;
    constexpr auto jackBorderSize = 8.f;

    addAndMakeVisible(outJack);
    outJack.setBounds(body.getCentreX() - (jackSize / 2),
                         body.getBottom() - jackSize - jackBorderSize,
                         jackSize, jackSize);

    jackBorder = outJack.getBounds().toFloat().expanded(jackBorderSize, jackBorderSize);

    connectListener(delay, EnvDSP::delayIn);
    connectListener(attack, EnvDSP::attackIn);
    connectListener(hold, EnvDSP::holdIn);
    connectListener(decay, EnvDSP::decayIn);
    connectListener(sustain, EnvDSP::sustainIn);
    connectListener(release, EnvDSP::releaseIn);

    setControlBounds({IOBoundsReduced.reduced(IOInsetX)});
    setVisualGradient();
    setEnvPath();
}

void EnvModuleComponent::connectListener(SiScSlider &slider, int control) {
    slider.onValueChange = [this, &slider, control] {
        if (onParameterChanged)
            onParameterChanged (getModuleID(), control, static_cast<float> (slider.getValue()));
        setEnvPath();
        repaint();
    };
}

void EnvModuleComponent::setControlBounds(const std::vector<juce::Rectangle<float>>& controlBounds) {
    const float leftX = controlBounds[0].getX();
    const float leftMiddleX = controlBounds[0].getX() + (controlBounds[0].getWidth() / 4);
    const float middleX = controlBounds[0].getCentreX();
    const float rightMiddleX = controlBounds[0].getX() + ((3 * controlBounds[0].getWidth()) / 4);
    const float bigControlCenterOffsetX = ((leftMiddleX - leftX) / 2) - (bigRotarySize / 2);
    constexpr float nudge = 15.f;
    constexpr float smallJackSize = 20.f;
    // const float smallControlCenterOffsetX = ((leftMiddleX - leftX) / 2) - (smallRotarySize / 2);

    const float topY = controlBounds[0].getY();
    const float middleY = controlBounds[0].getCentreY();

    addAndMakeVisible(triggerInJack);
    triggerInJack.setBounds(controlBounds[0].getCentreX() - (smallJackSize / 2),
                     topY + ((middleY - topY) / 2) - (smallJackSize / 2) - 5,
                    smallJackSize,
                    smallJackSize);
    inJackLabelBounds.setBounds(controlBounds[0].getCentreX() - 25,
                         topY + ((middleY - topY) / 2) - (smallJackSize / 2) + smallJackSize,
                         50,
                         15);

    setControlAndLabelVisAndBounds(delay, delayLabelBounds, leftX + bigControlCenterOffsetX, middleY, 0.35);
    setControlAndLabelVisAndBounds(attack, attackLabelBounds, leftX + bigControlCenterOffsetX, topY, 0.35);
    setControlAndLabelVisAndBounds(hold, holdLabelBounds, leftMiddleX - nudge + bigControlCenterOffsetX, topY, 0.35);
    setControlAndLabelVisAndBounds(decay, decayLabelBounds, middleX + nudge + bigControlCenterOffsetX, topY, 0.35);
    setControlAndLabelVisAndBounds(sustain, sustainLabelBounds, rightMiddleX + bigControlCenterOffsetX, topY);
    setControlAndLabelVisAndBounds(release, releaseLabelBounds, rightMiddleX + bigControlCenterOffsetX, middleY, 0.35);
}

void EnvModuleComponent::setControlAndLabelVisAndBounds(SiScSlider &control, juce::Rectangle<float> &labelBounds, const float x, const float y, float skewFactor) {
    addAndMakeVisible(control);
    control.setSkewFactor(skewFactor);
    control.setBounds(x,
                      y - 5,
                    bigRotarySize,
                    bigRotarySize);
    labelBounds.setBounds(x,
                         y + bigRotarySize - 13,
                         bigRotarySize,
                         15);
}

void EnvModuleComponent::setEnvPath() {
    const auto del = static_cast<float>(delay.getValue());
    const auto atk = static_cast<float>(attack.getValue());
    const auto hld = static_cast<float>(hold.getValue());
    const auto dec = static_cast<float>(decay.getValue());
    const auto sus = static_cast<float>(sustain.getValue());
    const auto rel = static_cast<float>(release.getValue());
    float visualLength = del + atk + hld + dec + rel;

    if (visualLength < 1) {
        visualLength = 1.f;
    }
    const float actualLength = innerEnvBounds.getWidth();

    const juce::Point startPt = {innerEnvBounds.getX() + ((del / visualLength) * actualLength),
                                 innerEnvBounds.getBottom()};
    const juce::Point atkPt = {innerEnvBounds.getX() + (((del + atk) / visualLength) * actualLength),
                               innerEnvBounds.getY()};
    const juce::Point hldPt = {innerEnvBounds.getX() + (((del + atk + hld) / visualLength) * actualLength),
                               innerEnvBounds.getY()};
    const juce::Point decPt = {innerEnvBounds.getX() + (((del + atk + hld + dec) / visualLength) * actualLength),
                               (innerEnvBounds.getHeight() * (1 - sus)) + innerEnvBounds.getY()};
    const juce::Point endPt = {innerEnvBounds.getRight(),
                               innerEnvBounds.getBottom()};

    envPath.clear();
    envPathClosed.clear();

    envPath.startNewSubPath(startPt);
    envPath.lineTo(atkPt);
    envPath.lineTo(hldPt);
    envPath.lineTo(decPt);
    envPath.lineTo(endPt); // visual path

    envPathClosed.startNewSubPath(startPt);
    envPathClosed.lineTo(atkPt);
    envPathClosed.lineTo(hldPt);
    envPathClosed.lineTo(decPt);
    envPathClosed.lineTo(endPt);
    envPathClosed.closeSubPath(); // closed path, used to clip gradient
}

void EnvModuleComponent::setHeaderDesignPath() {
    const auto startPt = headerBounds.getX() + (headerBounds.getHeight() / 2);

    headerDesignPath.clear();

    headerDesignPath.startNewSubPath(body.getX(), startPt);
    headerDesignPath.lineTo(body.getRight(), startPt);
}

void EnvModuleComponent::setVisualGradient() {
    state ?
    envGradient = juce::ColourGradient::vertical(visualColor.withMultipliedAlpha(0.4),
                                               colors.getColor(ThemeColors::canvasBG).withMultipliedAlpha(0.4),
                                                          envGradientRect) :
    envGradient = juce::ColourGradient::vertical(visualColor,
                                               colors.getColor(ThemeColors::canvasBG),
                                                          envGradientRect);
}

juce::Array<InJack *> EnvModuleComponent::getInJacks() {
    juce::Array<InJack*> inJacks;

    inJacks.add(&delay.getInJack());
    inJacks.add(&attack.getInJack());
    inJacks.add(&hold.getInJack());
    inJacks.add(&triggerInJack);
    inJacks.add(&decay.getInJack());
    inJacks.add(&sustain.getInJack());
    inJacks.add(&release.getInJack());

    return inJacks;
}

void EnvModuleComponent::setJackIDs(const int ID) {
    triggerInJack.setID(ID);
    delay.getInJack().setID(ID);
    attack.getInJack().setID(ID);
    hold.getInJack().setID(ID);
    decay.getInJack().setID(ID);
    sustain.getInJack().setID(ID);
    release.getInJack().setID(ID);

    outJack.setID(ID);
}
