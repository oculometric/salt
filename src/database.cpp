#include "database.h"

#include <cstring>
#include <fstream>

using namespace std;

Database::Database()
{
    artists[0] = Artist{ 0, "jimbo", { 33, 34 } };
    albums[33] = Album{ 33, "the 33rd album", Source{ SourceType::CD, "33rd Album" }, Storage{ false, "the_33rd_album" }, 12345, { 0 }, { 0, 1, 2 } };
    tracks[0] = Track{ 0, "track 0", Source{ SourceType::SAME_AS_ALBUM }, Storage{ false, "the_33rd_album/track_0.mp3" }, { 0 }, 33, 1 };
    tracks[1] = Track{ 1, "track 1", Source{ SourceType::SAME_AS_ALBUM }, Storage{ false, "the_33rd_album/track_1.mp3" }, { 0 }, 33, 2 };
    tracks[2] = Track{ 2, "track 2", Source{ SourceType::SAME_AS_ALBUM }, Storage{ false, "the_33rd_album/track_2.mp3" }, { 0 }, 33, 3 };
    albums[34] = Album{ 34, "the 34th album", Source{ SourceType::BANDCAMP, "https://bandcamp.com" }, Storage{ false, "34th_album" }, 11123, { 0, 2 }, { 3, 4 } };
    tracks[3] = Track{ 3, "track a", Source{ SourceType::SAME_AS_ALBUM }, Storage{ false, "34th_album/a.mp3" }, { 0 }, 34, 1 };
    tracks[4] = Track{ 4, "track b", Source{ SourceType::SAME_AS_ALBUM }, Storage{ false, "34th_album/b.mp3" }, { 0, 2 }, 34, 2 };

    saveDatabase("test_db.audb");
}

#pragma pack(push)
#pragma pack(1)
struct DatabaseHeader
{
    char signature[4];
    uint32_t num_artists;
    uint32_t first_artist_offset;
    uint32_t num_albums;
    uint32_t first_album_offset;
    uint32_t num_tracks;
    uint32_t first_track_offset;
};

struct ArtistHeader
{
    uint32_t id;
    uint32_t total_block_length;
    uint16_t name_length;
    uint16_t album_list_length;
};

struct AlbumHeader
{
    uint32_t id;
    uint32_t total_block_length;
    uint16_t name_length;
    uint8_t source_type;
    uint8_t storage_type;
    uint16_t source_length;
    uint16_t storage_length;
    uint32_t date;
    uint16_t artist_list_length;
    uint16_t track_list_length;
};

struct TrackHeader
{
    uint32_t id;
    uint32_t total_block_length;
    uint16_t name_length;
    uint8_t source_type;
    uint8_t storage_type;
    uint16_t source_length;
    uint16_t storage_length;
    uint16_t artist_list_length;
    uint16_t track_index;
    uint32_t album_id;
};
#pragma pack(pop)

bool Database::saveDatabase(string filename)
{
    ofstream file(filename);
    if (!file.is_open())
        return false;

    vector<uint8_t> artist_data;
    for (const auto& art : artists)
    {
        ArtistHeader hdr;
        hdr.id = art.first;
        hdr.name_length = art.second.name.length();
        hdr.album_list_length = art.second.albums.size();
        hdr.total_block_length = sizeof(ArtistHeader) + hdr.name_length + (hdr.album_list_length * sizeof(EntryID));
        
        uint32_t data_offset = artist_data.size();
        artist_data.resize(data_offset + hdr.total_block_length);
        *(ArtistHeader*)(artist_data.data() + data_offset) = hdr;
        data_offset += sizeof(ArtistHeader);
        memcpy(artist_data.data() + data_offset, art.second.name.data(), hdr.name_length);
        data_offset += hdr.name_length;
        memcpy(artist_data.data() + data_offset, art.second.albums.data(), hdr.album_list_length * sizeof(EntryID));
    }

    vector<uint8_t> album_data;
    for (const auto& alb : albums)
    {
        AlbumHeader hdr;
        hdr.id = alb.first;
        hdr.name_length = alb.second.name.length();
        hdr.source_type = (uint8_t)(alb.second.source.type);
        hdr.storage_type = (uint8_t)(alb.second.storage.is_external);
        hdr.source_length = alb.second.source.reference.length();
        hdr.storage_length = alb.second.storage.path.length();
        hdr.date = alb.second.date;
        hdr.artist_list_length = alb.second.artists.size();
        hdr.track_list_length = alb.second.tracks.size();
        hdr.total_block_length = sizeof(AlbumHeader)
            + hdr.name_length
            + hdr.source_length
            + hdr.storage_length
            + (hdr.artist_list_length * sizeof(EntryID))
            + (hdr.track_list_length * sizeof(EntryID));
        
        uint32_t data_offset = album_data.size();
        album_data.resize(data_offset + hdr.total_block_length);
        *(AlbumHeader*)(album_data.data() + data_offset) = hdr;
        data_offset += sizeof(AlbumHeader);
        memcpy(album_data.data() + data_offset, alb.second.name.data(), hdr.name_length);
        data_offset += hdr.name_length;
        memcpy(album_data.data() + data_offset, alb.second.source.reference.data(), hdr.source_length);
        data_offset += hdr.source_length;
        memcpy(album_data.data() + data_offset, alb.second.storage.path.data(), hdr.storage_length);
        data_offset += hdr.storage_length;
        memcpy(album_data.data() + data_offset, alb.second.artists.data(), hdr.artist_list_length * sizeof(EntryID));
        data_offset += hdr.artist_list_length * sizeof(EntryID);
        memcpy(album_data.data() + data_offset, alb.second.tracks.data(), hdr.track_list_length * sizeof(EntryID));
    }

    vector<uint8_t> track_data;
    for (const auto& trk : tracks)
    {
        TrackHeader hdr;
        hdr.id = trk.first;
        hdr.name_length = trk.second.name.length();
        hdr.source_type = (uint8_t)(trk.second.source.type);
        hdr.storage_type = (uint8_t)(trk.second.storage.is_external);
        hdr.source_length = trk.second.source.reference.length();
        hdr.storage_length = trk.second.storage.path.length();
        hdr.artist_list_length = trk.second.artists.size();
        hdr.track_index = trk.second.track_index;
        hdr.album_id = trk.second.album;
        hdr.total_block_length = sizeof(AlbumHeader)
            + hdr.name_length
            + hdr.source_length
            + hdr.storage_length
            + (hdr.artist_list_length * sizeof(EntryID));
        
        uint32_t data_offset = track_data.size();
        track_data.resize(data_offset + hdr.total_block_length);
        *(TrackHeader*)(track_data.data() + data_offset) = hdr;
        data_offset += sizeof(track_data);
        memcpy(track_data.data() + data_offset, trk.second.name.data(), hdr.name_length);
        data_offset += hdr.name_length;
        memcpy(track_data.data() + data_offset, trk.second.source.reference.data(), hdr.source_length);
        data_offset += hdr.source_length;
        memcpy(track_data.data() + data_offset, trk.second.storage.path.data(), hdr.storage_length);
        data_offset += hdr.storage_length;
        memcpy(track_data.data() + data_offset, trk.second.artists.data(), hdr.artist_list_length * sizeof(EntryID));
        data_offset += hdr.artist_list_length * sizeof(EntryID);
    }

    DatabaseHeader hdr;
    hdr.signature[0] = 'A';
    hdr.signature[1] = 'D';
    hdr.signature[2] = 'B';
    hdr.signature[3] = 0x1;
    hdr.num_artists = artists.size();
    hdr.first_artist_offset = sizeof(DatabaseHeader);
    hdr.num_albums = albums.size();
    hdr.first_album_offset = hdr.first_artist_offset + artist_data.size();
    hdr.num_tracks = tracks.size();
    hdr.first_track_offset = hdr.first_album_offset + album_data.size();

    file.write((char*)(&hdr), sizeof(DatabaseHeader));
    file.write((char*)artist_data.data(), artist_data.size());
    file.write((char*)album_data.data(), album_data.size());
    file.write((char*)track_data.data(), track_data.size());

    file.close();

    return true;
}
