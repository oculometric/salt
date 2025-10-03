#include "ui_library.h"

static const char* library_page_str = 
R"V0G0N(
VerticalBox (children = {
    TabContainer : "tabs" (children = 
    {
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = ListView : "artist_list" (elements = { "first artist", "second" }, show_numbers = 0, enabled = 0)),
            VerticalDivider (),
            VerticalBox (children = {
                TextArea : "artist_detail_area" (text = "artist details go here"),
                ListView : "artist_album_list" (elements = { "the 33rd album", "the 34th album" })
            })
        }),
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = ListView : "album_list" (elements = { "first album", "second" }, show_numbers = 0)),
            VerticalDivider (),
            TextArea : "album_detail_area" (text = "album details go here")
        }),
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = ListView : "playlist_list" (elements = { "first playlist", "second", "third" }, show_numbers = 0, enabled = 0)),
            VerticalDivider (),
            TextArea : "playlist_detail_area" (text = "playlist details go here")
        }),
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = ListView : "track_list" (elements = { "first track", "second" }, show_numbers = 0, enabled = 0)),
            VerticalDivider (),
            TextArea : "track_detail_area" (text = "track details go here")
        })
    }, tab_titles = { "artists", "albums", "playlists", "tracks" }, current_tab = 1),
    Label (text = "[ctrl+F] search  |  [ctrl+A] import  |  [ctrl+M] modify  |  [ctrl+E] export  |  [ctrl+,] config")

    // TODO: add a 'refresh' button/keybind
    // TODO: the import shortcut will change to 'create' when the playlists tab is selected, and will not be visible when the artists tab is selected
    // TODO: the search and import shortcuts will jump to their own pages
    // TODO: the export shortcut will only be visible if playlists is selected
    // TODO: there will be a summary of how many albums you own
    // TODO: each tab child will show different stuff depending whether its albums, artists, playlists, or tracks
    // TODO: the config page will show initially or when the config shortcut is pressed
})
)V0G0N";

void LibraryUI::render()
{
    library_page->framerate(24);
    library_page->checkInput();
    library_page->render();
}

LibraryUI::LibraryUI()
{
    stui::LayoutReader reader;
    library_page = reader.readPage(library_page_str);

    tabs = library_page->get<stui::TabContainer>("tabs");
    tabs->setEnabled(true); // force update the enabled children
    tabs->callback = LibraryUI::libraryTabChanged;
    
    artist_list = library_page->get<stui::ListView>("artist_list");
    artist_detail_area = library_page->get<stui::TextArea>("artist_detail_area");
    artist_album_list = library_page->get<stui::ListView>("artist_album_list");
    album_list = library_page->get<stui::ListView>("album_list");
    playlist_list = library_page->get<stui::ListView>("playlist_list");
    track_list = library_page->get<stui::ListView>("track_list");

    library_page->focusable_component_sequence = { tabs, artist_list, artist_detail_area, artist_album_list, album_list, playlist_list, track_list };
}

LibraryUI::~LibraryUI()
{
    library_page->destroyAllComponents({ });
    delete library_page;
}

void LibraryUI::libraryTabChanged(int o, int n)
{

}
