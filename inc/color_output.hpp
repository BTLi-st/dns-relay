#pragma once
#ifndef COLOR_OUTPUT_HPP
#define COLOR_OUTPUT_HPP

#include <string>

enum Colors
{
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37
};

class Color
{
public:
    static std::string color(std::string str, int color)
    {
        return "\033[1;" + std::to_string(color) + "m" + str + "\033[0m";
    }
};

#endif