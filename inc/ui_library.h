#include "stui_script.h"

class Database;

class LibraryUI
{
private:
    Database* database;

    stui::Page* library_page;
    stui::TabContainer* tabs;

    stui::ListView* artist_list;
    stui::Label* artist_count_label;
    stui::TextArea* artist_detail_area;
    stui::ListView* artist_album_list;

    stui::ListView* album_list;
    stui::Label* album_count_label;

    stui::ListView* playlist_list;

    stui::ListView* track_list;
    stui::Label* track_count_label;

public:
    void render();

    LibraryUI(Database* _database);
    ~LibraryUI();

private:
    static void libraryTabChanged(int o, int n);
    static bool refreshShortcut();

    void updateCurrentPageDetails();
    void refreshArtists();
    void updateArtistDetails();
    void refreshAlbums();
    void updateAlbumDetails();
    void refreshPlaylists();
    void updatePlaylistDetails();
    void refreshTracks();
    void updateTrackDetails();
};