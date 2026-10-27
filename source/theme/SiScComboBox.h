#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "Jack.h"
#include "Colors.h"

class SiScComboBox : public juce::ComboBox {
public:
    SiScComboBox(Colors& colorsRef);

    [[nodiscard]] InJack& getInJack();

    InJack inJack;

private:
};