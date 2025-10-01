#include <iostream>
#define STUI_IMPLEMENTATION
#include "stui_extensions.h"

int main()
{
    std::cout << "hiyya" << std::endl;
    sleep(2);
    stui::Terminal::configure("Simple Audio Library Tool", 1.0f);
}