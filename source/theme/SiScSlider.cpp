#include "SiScSlider.h"

SiScSlider::SiScSlider(juce::Slider::SliderStyle sliderStyle, float startValue, float endValue, float defaultValue, float step, std::initializer_list<double> snapPoints, IndicatorType indicatorType) : snapValues(snapPoints), indicatorType(indicatorType) {
    setSliderStyle(sliderStyle);
    setRange(startValue, endValue, step);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    setValue(defaultValue);

    for (const auto& point : snapPoints) {
        if (defaultValue == point) {
            latchedValue = point;
        }
    }

    if (getInterval() < 1) {
        snapDistance = (endValue - startValue) / 20;
    } else {
        snapDistance = 0;
    }
}

double SiScSlider::snapValue(double attemptedValue, DragMode dragMode) {
    if (getInterval() > 0) {
        attemptedValue = floor(attemptedValue);
    }

    if (dragMode == notDragging || snapValues.empty())
        return attemptedValue;

    if (latchedValue.has_value()) {
        if (std::abs(attemptedValue - latchedValue.value()) < snapDistance) {
            prevValue = latchedValue.value();
            return latchedValue.value();
        }

        latchedValue.reset();
    }

    if (!getRotaryParameters().stopAtEnd) {
        const auto wrapThreshold = juce::MathConstants<double>::twoPi - 0.1;

        if (prevValue - attemptedValue > wrapThreshold) {
            latchedValue = 0.0;
            prevValue = 0.0;
            return 0.0;
        }

        if (attemptedValue - prevValue > wrapThreshold) {
            latchedValue = getMaximum();
            prevValue = getMaximum();
            return getMaximum();
        }
    }

    for (const auto& point : snapValues) {
        if (attemptedValue == point ||
            (prevValue < point && attemptedValue > point) ||
            (prevValue > point && attemptedValue < point)) {

            latchedValue = point;
            prevValue = point;
            return point;
        }
    }

    prevValue = attemptedValue;
    return attemptedValue;
}

std::vector<double> SiScSlider::getSnapValues() const {
    return snapValues;
}

std::optional<double> SiScSlider::getLatchedValue() const {
    return latchedValue;
}

IndicatorType SiScSlider::getIndicatorType() const {
    return indicatorType;
}
