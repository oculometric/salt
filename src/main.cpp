#include <iostream>
#define STUI_IMPLEMENTATION
#include "stui_script.h"

#include "database.h"
#include "ui.h"

int main()
{
    stui::Terminal::configure("Simple Audio Library Tool", 1.0f);
    stui::LayoutReader reader;
    stui::Page* explore_page = reader.readPage(explore_page_str);
    //explore_page->focusable_component_sequence = { (*explore_page)["art_but"], (*explore_page)["alb_but"], (*explore_page)["trk_but"], (*explore_page)["search_but"], (*explore_page)["import_but"], (*explore_page)["config_but"] };
    explore_page->focusable_component_sequence = { (*explore_page)["menu"] };
    while (true)
    {
        explore_page->framerate(24);
        explore_page->checkInput();
        explore_page->render();
    }
    //Database d;
}
