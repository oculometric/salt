#include <iostream>
#define STUI_IMPLEMENTATION
#include "stui_script.h"

#include "database.h"
#include "ui.h"

stui::TabContainer* tabs;
stui::ListView* artist_list;
stui::ListView* album_list;
stui::ListView* playlist_list;
stui::ListView* track_list;

void libraryTabChanged(int old_tab, int new_tab)
{
    switch (old_tab)
    {
        case 0: artist_list->enabled = false; break;
        case 1: album_list->enabled = false; break;
        case 2: playlist_list->enabled = false; break;
        case 3: track_list->enabled = false; break;
    }

    switch (new_tab)
    {
        case 0: artist_list->enabled = true; break;
        case 1: album_list->enabled = true; break;
        case 2: playlist_list->enabled = true; break;
        case 3: track_list->enabled = true; break;
    }
}

int main()
{
    stui::Terminal::configure("Simple Audio Library Tool", 1.0f);
    stui::LayoutReader reader;
    stui::Page* library_page = reader.readPage(library_page_str);
    artist_list = library_page->get<stui::ListView>("artist_list");
    album_list = library_page->get<stui::ListView>("album_list");
    playlist_list = library_page->get<stui::ListView>("playlist_list");
    track_list = library_page->get<stui::ListView>("track_list");
    tabs = library_page->get<stui::TabContainer>("tabs");
    library_page->focusable_component_sequence = { tabs, artist_list, album_list, playlist_list, track_list };
    tabs->callback = libraryTabChanged;
    while (true)
    {
        library_page->framerate(24);
        library_page->checkInput();
        library_page->render();
    }
    //Database d;
}
