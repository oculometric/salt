const char* explore_page_str = 
R"V0G0N(
HorizontalBox (children = {
    SizeLimiter (max_size = [24, -1], child = VerticalBox (children = {
        Label (text = "menu", alignment = 0),
        ListView : "menu" (elements = { "artists", "albums", "tracks", "search", "import", "configure" })
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