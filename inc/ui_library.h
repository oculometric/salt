#include "stui_script.h"

class LibraryUI
{
private:
    stui::Page* library_page;
    stui::TabContainer* tabs;
    stui::ListView* artist_list;
    stui::TextArea* artist_detail_area;
    stui::ListView* artist_album_list;
    stui::ListView* album_list;
    stui::ListView* playlist_list;
    stui::ListView* track_list;

public:
    void render();

    LibraryUI();
    ~LibraryUI();

private:
    static void libraryTabChanged(int o, int n);
    void refreshArtists();
    void refreshAlbums();
    void refreshPlaylists();
    void refreshTracks();
};