#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <optional>
#include "Jack.h"
#include "Colors.h"

enum class IndicatorType : size_t {
    none,
    circle,
    line,
};

enum class SliderType : size_t {
    standard,
    modulation,
};

class SiScSlider : public juce::Slider {
public:
    SiScSlider(SliderStyle sliderStyle,
        SliderType sliderType,
        float startValue,
        float endValue,
        float defaultValue,
        Colors &colorsRef,
        float step = 0,
        std::initializer_list<double> snapPoints = {},
        IndicatorType indicatorType = IndicatorType::none);

    void mouseMove (const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) override;

    double snapValue(double attemptedValue, DragMode dragMode) override;

    [[nodiscard]] std::vector<double> getSnapValues() const;
    [[nodiscard]] std::optional<double> getLatchedValue() const;
    [[nodiscard]] IndicatorType getIndicatorType() const;
    [[nodiscard]] SliderType getSliderType() const;
    [[nodiscard]] InJack& getInJack();

    InJack inJack;
private:
    std::vector<double> snapValues;
    IndicatorType indicatorType;
    SliderType sliderType;
    double snapDistance;
    double prevValue;
    std::optional<double> latchedValue;
    float scrollDeltaY = 0.f;
};
