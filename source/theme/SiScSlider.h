#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <optional>

enum class IndicatorType : size_t {
    none,
    circle,
    line,
};

class SiScSlider : public juce::Slider {
public:
    SiScSlider(SliderStyle sliderStyle,
        float startValue,
        float endValue,
        float defaultValue,
        float step = 0,
        std::initializer_list<double> snapPoints = {},
        IndicatorType indicatorType = IndicatorType::none);

    double snapValue(double attemptedValue, DragMode dragMode) override;

    [[nodiscard]] std::vector<double> getSnapValues() const;
    [[nodiscard]] std::optional<double> getLatchedValue() const;
    [[nodiscard]] IndicatorType getIndicatorType() const;

private:
    std::vector<double> snapValues;
    IndicatorType indicatorType;
    double snapDistance;
    double prevValue;
    std::optional<double> latchedValue;
};
