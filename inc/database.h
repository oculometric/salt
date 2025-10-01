#include <string>
#include <stdint.h>
#include <vector>
#include <map>

enum SourceType
{
    CD,
    BANDCAMP,
    SOUNDCLOUD,
    SAME_AS_ALBUM,
    OTHER
};

struct Source
{
    SourceType type;
    std::string reference;
};

struct Storage
{
    bool is_external;
    std::string path;
};

typedef uint32_t EntryID;

// TODO: cache pointers to structs reffed by IDs for faster lookup?
struct Artist
{
    EntryID id;
    std::string name;

    std::vector<EntryID> albums;
};

struct Album
{
    EntryID id;
    std::string name;
    Source source;
    Storage storage;

    uint32_t date;
    std::vector<EntryID> artists;
    std::vector<EntryID> tracks;
};

struct Track
{
    EntryID id;
    std::string name;
    Source source;
    Storage storage;

    std::vector<EntryID> artists;
    EntryID album;
    uint track_index;
};

class Database
{
private:
    std::map<EntryID, Artist> artists;
    std::map<EntryID, Album> albums;
    std::map<EntryID, Track> tracks;

public:
    bool loadDatabase(std::string filename);
    bool saveDatabase(std::string filename);

    std::vector<Artist*> enumerateArists();
    std::vector<Album*> enumerateAlbums();
    std::vector<Track*> enumerateTracks();

    std::vector<Album*> getAlbumsByArtist(EntryID artist);
    std::vector<Album*> getAlbumsByArtist(Artist* artist);

    std::vector<Track*> getTracksByArtist(EntryID artist);
    std::vector<Track*> getTracksByArtist(Artist* artist);

    std::vector<Track*> getTracksInAlbum(EntryID album);
    std::vector<Track*> getTracksInAlbum(Album* album);

    Album* getAlbumOfTrack(EntryID track);
    Album* getAlbumOfTrack(Track* track);

    std::vector<Artist*> getArtistsOfTrack(EntryID track);
    std::vector<Artist*> getArtistsOfTrack(Track* track);

    std::vector<Artist*> getArtistsofAlbum(EntryID album);
    std::vector<Artist*> getArtistsofAlbum(Album* album);

    std::string getArtistsSummary(Track* track);
    std::string getArtistsSummary(Album* track);

    Database();
};

// Database file structure:
// 
// header:
//      signature (ADB 0x1)             4 bytes
//      number of artists in store      4 bytes
//      first artist block offset       4 bytes
//      number of albums in store       4 bytes
//      first album block offset        4 bytes
//      number of tracks in store       4 bytes
//      first track block offset        4 bytes
//      playlist header offset          4 bytes
//      event history offset            4 bytes
// ...
// artist block:
//      id                              4 bytes
//      total block length              4 bytes
//      name length                     2 bytes
//      album list length               2 bytes
//      name                            n bytes
//      album ids                     m*4 bytes
// ...
// album block:
//      id                              4 bytes
//      total block length              4 bytes
//      name length                     2 bytes
//      source type                     1 byte
//      storage type                    1 byte
//      source length                   2 bytes
//      storage length                  2 bytes
//      date                            4 bytes
//      artist list length              2 bytes
//      track list length               2 bytes
//      name                            n bytes
//      source                          m bytes
//      storage                         l bytes
//      artist ids                      o*4 bytes
//      track ids                       p*4 bytes
// ...
// track block:
//      id                              4 bytes
//      total block length              4 bytes
//      name length                     2 bytes
//      source type                     1 byte
//      storage type                    1 byte
//      source length                   2 bytes
//      storage length                  2 bytes
//      artist list length              2 bytes
//      track index                     2 bytes
//      album id                        4 bytes
//      name                            n bytes
//      source                          m bytes
//      storage                         l bytes
//      artist ids                      o*4 bytes
// ...
// TODO: playlist blocks, event history