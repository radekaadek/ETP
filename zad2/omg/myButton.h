#pragma once

#include <QPushButton>
#include <string>

// a class that is QPushButton but with a string position member
class myButton : public QPushButton
{
    Q_OBJECT
    std::string position;
public:
    myButton(QWidget* parent = nullptr) : QPushButton(parent) {}
    std::string getPosition() const;
    void setPosition(const std::string &value);
};


// {
//     std::string position;
// public:
//     std::string getPosition() const;
//     void setPosition(const std::string &value);
// }