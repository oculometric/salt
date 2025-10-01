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

    loadDatabase("test_db.audb");
}

#pragma pack(push)
#pragma pack(1)
struct DatabaseHeader
{
    uint32_t signature = 0x41444201;
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
    ofstream file(filename, ios::binary);
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

bool Database::loadDatabase(string filename)
{
    artists.clear();
    albums.clear();
    tracks.clear();

    ifstream file(filename, ios::ate | ios::binary);
    if (!file.is_open())
        return false;

    size_t file_size = file.tellg();
    file.seekg(ios::beg);
    uint8_t* file_data = new uint8_t[file_size];
    file.read((char*)file_data, file_size);
    file.close();

    if (file_size < sizeof(DatabaseHeader))
    {
        delete[] file_data;
        return false;
    }

    DatabaseHeader* hdr = (DatabaseHeader*)file_data;
    if (hdr->signature != 0x41444201
    || hdr->first_artist_offset >= file_size
    || hdr->first_album_offset >= file_size
    || hdr->first_track_offset >= file_size)
    {
        delete[] file_data;
        return false;
    }

    uint8_t* file_data_end = file_data + file_size;

    uint8_t* cur_hdr = file_data + hdr->first_artist_offset;
    for (uint32_t i = 0; i < hdr->num_artists; ++i)
    {
        if (cur_hdr + sizeof(ArtistHeader) > file_data_end)
            break;
        ArtistHeader* art_hdr = (ArtistHeader*)cur_hdr;
        if (cur_hdr + art_hdr->total_block_length > file_data_end)
            break;
        
        cur_hdr += sizeof(ArtistHeader);
        string name; name.resize(art_hdr->name_length, '\0');
        memcpy(name.data(), cur_hdr, art_hdr->name_length);
        cur_hdr += art_hdr->name_length;
        vector<EntryID> album_ids; album_ids.resize(art_hdr->album_list_length, 0);
        memcpy(album_ids.data(), cur_hdr, art_hdr->album_list_length * sizeof(EntryID));

        artists[art_hdr->id] = Artist{ art_hdr->id, name, album_ids };
        
        cur_hdr = ((uint8_t*)art_hdr) + art_hdr->total_block_length;
    }

    cur_hdr = file_data + hdr->first_album_offset;
    for (uint32_t i = 0; i < hdr->num_albums; ++i)
    {
        if (cur_hdr + sizeof(AlbumHeader) > file_data_end)
            break;
        AlbumHeader* alb_hdr = (AlbumHeader*)cur_hdr;
        if (cur_hdr + alb_hdr->total_block_length > file_data_end)
            break;
        
        cur_hdr += sizeof(AlbumHeader);
        string name; name.resize(alb_hdr->name_length, '\0');
        memcpy(name.data(), cur_hdr, alb_hdr->name_length);
        cur_hdr += alb_hdr->name_length;
        string source_path; source_path.resize(alb_hdr->source_length, '\0');
        memcpy(source_path.data(), cur_hdr, alb_hdr->source_length);
        cur_hdr += alb_hdr->source_length;
        string storage_path; storage_path.resize(alb_hdr->storage_length, '\0');
        memcpy(storage_path.data(), cur_hdr, alb_hdr->storage_length);
        cur_hdr += alb_hdr->storage_length;
        vector<EntryID> artist_ids; artist_ids.resize(alb_hdr->artist_list_length, 0);
        memcpy(artist_ids.data(), cur_hdr, alb_hdr->artist_list_length * sizeof(EntryID));
        cur_hdr += alb_hdr->artist_list_length * sizeof(EntryID);
        vector<EntryID> track_ids; track_ids.resize(alb_hdr->track_list_length, 0);
        memcpy(track_ids.data(), cur_hdr, alb_hdr->track_list_length * sizeof(EntryID));

        albums[alb_hdr->id] = Album{ alb_hdr->id, name, Source{ (SourceType)alb_hdr->source_type, source_path }, Storage{ (bool)alb_hdr->storage_type, storage_path }, alb_hdr->date, artist_ids, track_ids };
        
        cur_hdr = ((uint8_t*)alb_hdr) + alb_hdr->total_block_length;
    }

    cur_hdr = file_data + hdr->first_track_offset;
    for (uint32_t i = 0; i < hdr->num_tracks; ++i)
    {
        if (cur_hdr + sizeof(TrackHeader) > file_data_end)
            break;
        TrackHeader* trk_hdr = (TrackHeader*)cur_hdr;
        if (cur_hdr + trk_hdr->total_block_length > file_data_end)
            break;
        
        cur_hdr += sizeof(TrackHeader);
        string name; name.resize(trk_hdr->name_length, '\0');
        memcpy(name.data(), cur_hdr, trk_hdr->name_length);
        cur_hdr += trk_hdr->name_length;
        string source_path; source_path.resize(trk_hdr->source_length, '\0');
        memcpy(source_path.data(), cur_hdr, trk_hdr->source_length);
        cur_hdr += trk_hdr->source_length;
        string storage_path; storage_path.resize(trk_hdr->storage_length, '\0');
        memcpy(storage_path.data(), cur_hdr, trk_hdr->storage_length);
        cur_hdr += trk_hdr->storage_length;
        vector<EntryID> artist_ids; artist_ids.resize(trk_hdr->artist_list_length, 0);
        memcpy(artist_ids.data(), cur_hdr, trk_hdr->artist_list_length * sizeof(EntryID));
        cur_hdr += trk_hdr->artist_list_length * sizeof(EntryID);

        tracks[trk_hdr->id] = Track{ trk_hdr->id, name, Source{ (SourceType)trk_hdr->source_type, source_path }, Storage{ (bool)trk_hdr->storage_type, storage_path }, artist_ids, trk_hdr->album_id, trk_hdr->track_index };
        
        cur_hdr = ((uint8_t*)trk_hdr) + trk_hdr->total_block_length;
    }

    delete[] file_data;

    return true;
}