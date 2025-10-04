#include <iostream>
#define STUI_IMPLEMENTATION
#include "stui_script.h"

#include "database.h"
#include "ui_library.h"

int main()
{
    stui::Terminal::configure("Simple Audio Library Tool", 1.0f);
    Database d;

    LibraryUI library_ui(&d);
    while (true)
        library_ui.render();
}
