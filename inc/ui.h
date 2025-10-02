const char* explore_page_str = 
R"V0G0N(
HorizontalBox (children = {
    SizeLimiter (max_size = [24, -1], child = VerticalBox (children = {
        Label (text = "menu", alignment = 0),
        ListView : "menu" (elements = { "artists", "albums", "tracks", "search", "import", "configure" }, show_numbers = 0)
        // Button : "art_but" (text = "artists"),
        // Button : "alb_but" (text = "albums"),
        // Button : "trk_but" (text = "tracks"),
        // Button : "search_but" (text = "search"),
        // Button : "import_but" (text = "import"),
        // Button : "config_but" (text = "configure")
    })),
    BorderedBox : "container_box" (name = "...", child = SizeLimiter(max_size = [-1, -1]))
})
)V0G0N";

const char* library_page_str = 
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

    // TODO: the import shortcut will change to 'create' when the playlists tab is selected, and will not be visible when the artists tab is selected
    // TODO: the search and import shortcuts will jump to their own pages
    // TODO: the export shortcut will only be visible if playlists is selected
    // TODO: there will be a summary of how many albums you own
    // TODO: each tab child will show different stuff depending whether its albums, artists, playlists, or tracks
    // TODO: the config page will show initially or when the config shortcut is pressed
})
)V0G0N";