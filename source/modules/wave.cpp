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
}

void WaveModuleComponent::onPathsReady() {
    setHeaderSinPath();

    headerBounds.removeFromRight(105);
    headerBounds.removeFromLeft(105);
}

void WaveModuleComponent::setHeaderSinPath() {
    const auto startPt = headerBounds.getX() + (headerBounds.getHeight() / 2) + 2;
    const auto amplitude = 3;

    headerSinPath.startNewSubPath(body.getX(), startPt);
    for (auto x = static_cast<int>(body.getX()); x < static_cast<int>(body.getWidth()) + body.getX(); x++) {
        headerSinPath.lineTo(static_cast<float>(x), amplitude * std::sin(0.25 * x) + startPt);
    }
}
