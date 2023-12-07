#include "myButton.h"

std::string myButton::getPosition() const
{
    return position;
}

void myButton::setPosition(const std::string &value)
{
    position = value;
}