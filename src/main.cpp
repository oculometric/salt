#include <iostream>
#define STUI_IMPLEMENTATION
#include "stui_script.h"

#include "database.h"
#include "ui.h"

stui::TabContainer* tabs;
stui::ListView* artist_list;
stui::TextArea* artist_detail_area;
stui::ListView* artist_album_list;
stui::ListView* album_list;
stui::ListView* playlist_list;
stui::ListView* track_list;

void libraryTabChanged(int old_tab, int new_tab)
{
}

int main()
{
    stui::Terminal::configure("Simple Audio Library Tool", 1.0f);

    stui::LayoutReader reader;
    stui::Page* library_page = reader.readPage(library_page_str);
    artist_list = library_page->get<stui::ListView>("artist_list");
    artist_detail_area = library_page->get<stui::TextArea>("artist_detail_area");
    artist_album_list = library_page->get<stui::ListView>("artist_album_list");
    album_list = library_page->get<stui::ListView>("album_list");
    playlist_list = library_page->get<stui::ListView>("playlist_list");
    track_list = library_page->get<stui::ListView>("track_list");
    tabs = library_page->get<stui::TabContainer>("tabs");
    library_page->focusable_component_sequence = { tabs, artist_list, artist_detail_area, artist_album_list, album_list, playlist_list, track_list };
    tabs->setEnabled(true); // force update the enabled children
    tabs->callback = libraryTabChanged;
    while (true)
    {
        library_page->framerate(24);
        library_page->checkInput();
        library_page->render();
    }
    //Database d;
}
