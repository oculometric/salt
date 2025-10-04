#include "ui_library.h"

#include <chrono>

#include "database.h"

static const char* library_page_str = 
R"V0G0N(
VerticalBox (children = {
    TabContainer : "tabs" (children = 
    {
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = VerticalBox (children = {
                ListView : "artist_list" (elements = { "first artist", "second" }, show_numbers = 0, enabled = 0),
                Label : "artist_count" (text = "1/2", alignment = 1)
            })),
            VerticalDivider (),
            VerticalBox (children = {
                BorderedBox (child = PropertyView : "artist_detail_area" (keys = { "artist" }), name = "details"),
                BorderedBox (child = ListView : "artist_album_list" (elements = { "theaeaegaeg 33rd album", "the 34taegaegah album" }), name = "albums")
            })
        }),
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = VerticalBox (children = {
                ListView : "album_list" (elements = { "first album", "second" }, show_numbers = 0),
                Label : "album_count" (text = "1/10", alignment = 1)
            })),
            VerticalDivider (),
            VerticalBox (children = {
                BorderedBox (child = PropertyView : "album_detail_area" (keys = { "album", "artist", "date", "duration", "source" }), name = "details"),
                BorderedBox (child = ListView : "album_track_list" (elements = { "track 1 trillion" }), name = "tracks")
            })
        }),
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = ListView : "playlist_list" (elements = { "first playlist", "second", "third" }, show_numbers = 0, enabled = 0)),
            VerticalDivider (),
            TextArea : "playlist_detail_area" (text = "playlist details go here")
        }),
        HorizontalBox (children = {
            SizeLimiter (max_size = [24, -1], child = VerticalBox (children = {
                ListView : "track_list" (elements = { "first track", "second" }, show_numbers = 0, enabled = 0),
                Label : "track_count" (text = "1/1", alignment = 1)
            })),
            VerticalDivider (),
            TextArea : "track_detail_area" (text = "track details go here")
        })
    }, tab_titles = { "artists", "albums", "playlists", "tracks" }, current_tab = 1),
    Label (text = "[ctrl+R] refresh  |  [ctrl+F] search  |  [ctrl+A] import  |  [ctrl+M] modify  |  [ctrl+E] export  |  [ctrl+,] config")

    // TODO: the import shortcut will change to 'create' when the playlists tab is selected, and will not be visible when the artists tab is selected
    // TODO: the search and import shortcuts will jump to their own pages
    // TODO: the export shortcut will only be visible if playlists is selected
    // TODO: each tab child will show different stuff depending whether its albums, artists, playlists, or tracks
    // TODO: the config page will show initially or when the config shortcut is pressed
})
)V0G0N";

static LibraryUI* ui = nullptr;

void LibraryUI::render()
{
    library_page->framerate(24);
    if (library_page->checkInput())
    {
        updateCurrentPageDetails();
    }
    library_page->render();
}

LibraryUI::LibraryUI(Database* _database)
{
    ui = this;
    database = _database;
    stui::LayoutReader reader;
    library_page = reader.readPage(library_page_str);

    tabs = library_page->get<stui::TabContainer>("tabs");
    tabs->setEnabled(true); // force update the enabled children
    tabs->callback = LibraryUI::libraryTabChanged;
    
    artist_list = library_page->get<stui::ListView>("artist_list");
    artist_count_label = library_page->get<stui::Label>("artist_count");
    artist_detail_area = library_page->get<stui::PropertyView>("artist_detail_area");
    artist_album_list = library_page->get<stui::ListView>("artist_album_list");
    artist_album_list->callback = albumSelectedFromArtist;
    album_list = library_page->get<stui::ListView>("album_list");
    album_count_label = library_page->get<stui::Label>("album_count");
    album_detail_area = library_page->get<stui::PropertyView>("album_detail_area");
    album_track_list = library_page->get<stui::ListView>("album_track_list");
    playlist_list = library_page->get<stui::ListView>("playlist_list");
    track_list = library_page->get<stui::ListView>("track_list");
    track_count_label = library_page->get<stui::Label>("track_count");

    library_page->focusable_component_sequence = { tabs, 
        artist_list, artist_detail_area, artist_album_list, 
        album_list, album_detail_area, album_track_list,
        playlist_list, track_list };
    library_page->shortcuts = 
    {
        stui::Input::Shortcut{ stui::Input::Key{ 'R', stui::Input::CTRL }, LibraryUI::refreshShortcut }
        //stui::Input::Shortcut{ stui::Input::Key{ 'F', stui::Input::CTRL }, nullptr }
        //stui::Input::Shortcut{ stui::Input::Key{ 'A', stui::Input::CTRL }, nullptr }
        //stui::Input::Shortcut{ stui::Input::Key{ 'M', stui::Input::CTRL }, nullptr }
        //stui::Input::Shortcut{ stui::Input::Key{ 'E', stui::Input::CTRL }, nullptr }
        //stui::Input::Shortcut{ stui::Input::Key{ ',', stui::Input::CTRL }, nullptr }
    };

    refreshShortcut();
}

LibraryUI::~LibraryUI()
{
    library_page->destroyAllComponents({ });
    delete library_page;
    ui = nullptr;
}

void LibraryUI::libraryTabChanged(int o, int n)
{
    //ui->updateCurrentPageDetails();
}

void LibraryUI::albumSelectedFromArtist()
{
    // FIXME: a lot of stuff should be cached!!!
    auto artist = ui->database->enumerateArists()[ui->artist_list->selected_index];
    auto albums = ui->database->getAlbumsByArtist(artist);
    auto selected_album = albums[ui->artist_album_list->selected_index];

    int i;
    for (i = 0; i < static_cast<int>(ui->album_list->elements.size()); i++)
        if (ui->album_list->elements[i] == selected_album->name)
            break;
    ui->album_list->selected_index = i;
    ui->tabs->setTab(1); // switch to album tab and set focus to be on album list
    ui->library_page->setFocusIndex(4);
}

bool LibraryUI::refreshShortcut()
{
    ui->refreshArtists();
    ui->refreshAlbums();
    ui->refreshPlaylists();
    ui->refreshTracks();
    ui->updateCurrentPageDetails();

    return true;
}

void LibraryUI::updateCurrentPageDetails()
{
    switch (tabs->current_tab)
    {
        case 0: updateArtistDetails();      break;
        case 1: updateAlbumDetails();       break;
        case 2: updatePlaylistDetails();    break;
        case 3: updateTrackDetails();       break;
    }
}

void LibraryUI::refreshArtists()
{
    // TODO: sorting!
    auto artists = database->enumerateArists();

    vector<string> artist_names;
    artist_list->elements.clear();
    for (auto artist : artists)
        artist_names.push_back(artist->name);
    
    artist_list->elements = artist_names;
}

void LibraryUI::updateArtistDetails()
{
    artist_count_label->text = to_string(artist_list->selected_index + 1) + '/' + to_string(artist_list->elements.size());

    auto artist = database->enumerateArists()[artist_list->selected_index];

    artist_detail_area->elements["artist"] = artist->name;
    
    auto albums = database->getAlbumsByArtist(artist);
    artist_album_list->elements.clear();
    for (auto album : albums)
        artist_album_list->elements.push_back(album->name);
}

void LibraryUI::refreshAlbums()
{
    // TODO: sorting!
    auto albums = database->enumerateAlbums();

    vector<string> album_names;
    for (auto album : albums)
        album_names.push_back(album->name);
    
    album_list->elements = album_names;
}

void LibraryUI::updateAlbumDetails()
{
    album_count_label->text = to_string(album_list->selected_index + 1) + '/' + to_string(album_list->elements.size());

    auto album = database->enumerateAlbums()[album_list->selected_index];
    
    auto tracks = database->getTracksInAlbum(album);
    album_track_list->elements.clear();
    for (auto track : tracks)
        album_track_list->elements.push_back(track->name);

    album_detail_area->elements["album"] = album->name;

    string artist_str;
    auto artists = database->getArtistsofAlbum(album);
    for (auto artist : artists)
    {
        artist_str += artist->name;
        artist_str += ", ";
    }
    artist_str.pop_back();
    artist_str.pop_back();
    album_detail_area->elements["artist"] = artist_str;

    album_detail_area->elements["date"] = to_string(album->date);

    album_detail_area->elements["source"] = album->source.reference;
}

void LibraryUI::refreshPlaylists()
{
    // TODO:
}

void LibraryUI::updatePlaylistDetails()
{
    // TODO:
}

void LibraryUI::refreshTracks()
{
    // TODO: sorting!
    auto tracks = database->enumerateTracks();

    vector<string> track_names;
    for (auto track : tracks)
        track_names.push_back(track->name);
    
    track_list->elements = track_names;
}

void LibraryUI::updateTrackDetails()
{
    track_count_label->text = to_string(track_list->selected_index + 1) + '/' + to_string(track_list->elements.size());

    // TODO: populate right-panel info about track
}
