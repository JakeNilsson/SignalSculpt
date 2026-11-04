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
        g.reduceClipRegion (envPathClosed);
        g.fillRoundedRectangle (envGradientRect, 5); // clips gradient to the closed env path
    }

    g.setColour(visualColor);
    {
        juce::Graphics::ScopedSaveState clip (g);
        g.reduceClipRegion (envBounds.toNearestInt());
        g.strokePath(envPath, juce::PathStrokeType (2.f)); // clips path itself to the env BG
    }
}

void EnvModuleComponent::drawIOSection(juce::Graphics &g) const {
    g.setColour(statusColor);
    g.strokePath(IOPath, juce::PathStrokeType(1.f)); // outline

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillRect(IOTextBounds); // BG for IO Text

    g.setColour(colors.getColor(ThemeColors::tabBG));
    g.fillEllipse(jackBorder); // BG for OutJack

    g.setColour(statusColor);
    g.setFont(moduleFont.withPointHeight(16.f));
    g.drawText(io.getText(), IOBounds, juce::Justification::centredTop); // IO Text
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

    envGradientRect.setBounds(envBounds.getX(), envBounds.getY(), envBounds.getWidth(), envBounds.getHeight());

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
    constexpr auto jackBorderSz = 8.f;

    addAndMakeVisible(outJack);
    outJack.setBounds(body.getCentreX() - (jackSize / 2),
                         body.getBottom() - jackSize - jackBorderSz,
                         jackSize, jackSize);

    jackBorder = outJack.getBounds().toFloat().expanded(jackBorderSz, jackBorderSz);

    setVisualGradient();

    setEnvPath();
}

void EnvModuleComponent::setEnvPath() {
    const juce::Point startPt = {envBounds.getX(), envBounds.getBottom()};
    const juce::Point endPt = {envBounds.getRight(), envBounds.getBottom()};

    envPath.startNewSubPath(startPt);
    envPath.lineTo(envBounds.getCentreX(), envBounds.getY());
    envPath.lineTo(endPt); // visual path

    envPathClosed.startNewSubPath(startPt);
    envPathClosed.lineTo(envBounds.getCentreX(), envBounds.getY());
    envPathClosed.lineTo(endPt);
    envPathClosed.closeSubPath(); // closed path, used to clip gradient
}

void EnvModuleComponent::setHeaderDesignPath() {
    const auto startPt = headerBounds.getX() + (headerBounds.getHeight() / 2);

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
    return juce::Array<InJack*> {};
}

void EnvModuleComponent::setJackIDs(const int ID) {
    outJack.setID(ID);
}
