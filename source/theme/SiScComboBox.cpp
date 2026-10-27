#include "SiScComboBox.h"

SiScComboBox::SiScComboBox(Colors& colorsRef) : inJack(colorsRef, "sliderIn") {}

InJack& SiScComboBox::getInJack() {
    return inJack;
}