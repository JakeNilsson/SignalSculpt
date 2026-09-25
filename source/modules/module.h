#pragma once
#include "../theme/Colors.h"

/*class Module {
public:
    virtual ~Module() = default;

    virtual void process (juce::AudioBuffer<float>& buffer) = 0;
    [[nodiscard]] virtual juce::String getName() const = 0;
};*/

class ModuleComponent : public juce::Component {
public:
    explicit ModuleComponent(Colors &colorsRef,
                             const float blockSizeX,
                             const float blockSizeY,
                             const juce::String& name)
                          :  colors(colorsRef),
                             name(name, name),
                             blockSizeX(blockSizeX),
                             blockSizeY(blockSizeY) {
        statusColor = colors.getColor(ThemeColors::neutral);
    }

    ~ModuleComponent() override = default;

    void paint(juce::Graphics& g) override = 0;

    [[nodiscard]] float getBlockSizeX() const {
        return blockSizeX;
    };
    [[nodiscard]] float getBlockSizeY() const {
        return blockSizeY;
    };

    [[nodiscard]] float getDividerWidth() const {
        return dividerWidth;
    }

    [[nodiscard]] float getBorderWidth() const {
        return borderWidth;
    }

    void setPaths();
    virtual void onPathsReady() {};

    void drawStateButton(juce::Graphics &g, juce::Point<float> mousePos) const;

    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;

protected:
    Colors &colors;

    juce::Label name;
    juce::Label io{"I/O section", "Inputs & Output"};

    juce::Path headerPath;
    juce::Path stateButtonPath;
    juce::Path stateButtonBorderPath;

    juce::Rectangle<float> innerBounds;
    juce::Rectangle<float> headerBounds;
    juce::Rectangle<float> body;

    bool mouseDownOnButton = false;
    bool mouseOver = false;
    bool state = true; // true for on, false for off

    juce::Colour statusColor;
    juce::Colour visualColor;
    juce::Colour comboBoxTextColor;

    juce::Font moduleFont;

private:
    const float blockSizeX;
    const float blockSizeY;

    float headerHeight = 0.0f;
    const float dividerWidth = 4.f;
    const float borderWidth = 4.f;
};