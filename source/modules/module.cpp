#include "module.h"
#include "BinaryData.h"

void ModuleComponent::setPaths() {
    const auto bounds = getLocalBounds().toFloat();
    innerBounds = bounds.reduced(10.f, 10.f);

    const auto halfDividerWidth = dividerWidth / 2;
    const auto doubleDividerWidth = dividerWidth * 2;
    const auto halfBorderWidth = borderWidth / 2;
    const auto doubleBorderWidth = borderWidth * 2;
    headerHeight = (innerBounds.getHeight() / 7) - halfDividerWidth;

    headerPath.startNewSubPath(innerBounds.getTopRight());
    headerPath.lineTo(innerBounds.getRight(), innerBounds.getY() + headerHeight);
    headerPath.lineTo(innerBounds.getX(), innerBounds.getY() + headerHeight);
    headerPath.lineTo(innerBounds.getX() + headerHeight, innerBounds.getY());
    headerPath.closeSubPath();

    stateButtonPath.startNewSubPath(innerBounds.getTopLeft());
    stateButtonPath.lineTo(innerBounds.getX(), innerBounds.getY() + headerHeight - doubleDividerWidth - borderWidth);
    stateButtonPath.lineTo(innerBounds.getX() + headerHeight - doubleDividerWidth - borderWidth, innerBounds.getY());
    stateButtonPath.closeSubPath();

    stateButtonBorderPath.startNewSubPath(innerBounds.getX() + halfBorderWidth, innerBounds.getY() + halfBorderWidth);
    stateButtonBorderPath.lineTo(innerBounds.getX() + halfBorderWidth, innerBounds.getY() + headerHeight - doubleDividerWidth - doubleBorderWidth);
    stateButtonBorderPath.lineTo(innerBounds.getX() + headerHeight - doubleDividerWidth - doubleBorderWidth, innerBounds.getY() + halfBorderWidth);
    stateButtonBorderPath.closeSubPath();

    body.setBounds(innerBounds.getX(), innerBounds.getY(),
                   innerBounds.getWidth(), innerBounds.getHeight());
    body.removeFromTop(innerBounds.getY() + headerHeight - halfDividerWidth);

    headerBounds.setBounds(innerBounds.getX(),
                           innerBounds.getY(),
                           innerBounds.getWidth(),
                           innerBounds.getHeight() - body.getHeight() - doubleDividerWidth);

    auto baseTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::kantumruyProVar_ttf, BinaryData::kantumruyProVar_ttfSize
        );

    juce::FontVariableSetting regularSetting[] {
        { juce::FontFeatureTag("wght"), 400.0f }
    };

    auto regularTypeface = baseTypeface->cloneWithVariableSettings(regularSetting);

    auto fontOptions = juce::FontOptions(regularTypeface).withPointHeight(32.0f);
    moduleFont = juce::Font(fontOptions);

    visualColor = colors.getColor(ThemeColors::visuals);

    onPathsReady();
}

void ModuleComponent::drawStateButton(juce::Graphics &g, const juce::Point<float> mousePos) const {
    if (state) {
        g.setColour(statusColor);
        g.fillPath(stateButtonPath);

        if (stateButtonPath.contains(mousePos)) {
            mouseDownOnButton ?
            g.setColour(colors.getColor(ThemeColors::input)) :
            g.setColour(colors.getColor(ThemeColors::tabBG));

            g.strokePath(stateButtonBorderPath, juce::PathStrokeType(borderWidth));
        }
    } else {
        stateButtonPath.contains(mousePos) ?
            g.setColour(colors.getColor(ThemeColors::tabBG)) :
            g.setColour(statusColor);

        g.fillPath(stateButtonPath);

        mouseDownOnButton ?
        g.setColour(colors.getColor(ThemeColors::output)) :
        g.setColour(colors.getColor(ThemeColors::tabBG));

        g.strokePath(stateButtonBorderPath, juce::PathStrokeType(borderWidth));
    }
}

void ModuleComponent::mouseEnter(const juce::MouseEvent &event) {
    if (auto mousePos = event.getPosition().toFloat(); stateButtonPath.contains(mousePos)) {
        repaint();
    }
}

void ModuleComponent::mouseMove(const juce::MouseEvent &event) {
    auto mousePos = event.getPosition().toFloat();

    if (stateButtonPath.contains(mousePos) && !mouseOver) {
        mouseOver = true;
        repaint();
    } else if (!stateButtonPath.contains(mousePos) && mouseOver) {
        mouseOver = false;
        repaint();
    }
}

void ModuleComponent::mouseExit(const juce::MouseEvent&) {
    if (mouseOver) {
        mouseOver = false;
        repaint();
    }
}

void ModuleComponent::mouseDown(const juce::MouseEvent &event) {
    auto mousePos = event.getPosition().toFloat();
    if (stateButtonPath.contains(mousePos) && !mouseDownOnButton) {
        mouseDownOnButton = true;
        repaint();
    }
}

void ModuleComponent::mouseUp(const juce::MouseEvent&) {
    if (mouseDownOnButton) {
        mouseDownOnButton = false;
        if (state) {
            state = false;
            statusColor = colors.getColor(ThemeColors::canvasBG);
            comboBoxTextColor = colors.getColor(ThemeColors::tabBG);
            visualColor = colors.getColor(ThemeColors::tabBG);
        } else {
            state = true;
            statusColor = colors.getColor(ThemeColors::neutral);
            comboBoxTextColor = colors.getColor(ThemeColors::neutral);
            visualColor = colors.getColor(ThemeColors::visuals);
        }

        repaint();
    }
}