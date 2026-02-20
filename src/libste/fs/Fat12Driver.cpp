#include "Fat12Driver.hpp"
#include <cstring>
#include <fstream>
#include <algorithm>

namespace libste {

Fat12Driver::Fat12Driver(DiskHandler& disk) : disk_(disk) {}

uint16_t Fat12Driver::get_fat_entry(uint16_t cluster) {
    auto fat = disk_.get_sector(1);
    if (fat.empty()) return 0xFFF;
    size_t offset = (cluster * 3) / 2;
    if (cluster % 2 == 0) {
        return fat[offset] | ((fat[offset + 1] & 0x0F) << 8);
    } else {
        return (fat[offset] >> 4) | (fat[offset + 1] << 4);
    }
}

void Fat12Driver::set_fat_entry(uint16_t cluster, uint16_t value) {
    auto fat = disk_.get_sector(1);
    if (fat.empty()) return;
    size_t offset = (cluster * 3) / 2;
    if (cluster % 2 == 0) {
        fat[offset] = value & 0xFF;
        fat[offset + 1] = (fat[offset + 1] & 0xF0) | ((value >> 8) & 0x0F);
    } else {
        fat[offset] = (fat[offset] & 0x0F) | ((value << 4) & 0xF0);
        fat[offset + 1] = (value >> 4) & 0xFF;
    }
}

uint16_t Fat12Driver::find_free_cluster() {
    for (uint16_t c = 2; c < 1440; ++c) {
        if (get_fat_entry(c) == 0x000) return c;
    }
    return 0;
}

std::vector<DirEntry> Fat12Driver::list_root_directory() {
    std::vector<DirEntry> entries;
    for (int s = 11; s <= 17; ++s) {
        auto sector = disk_.get_sector(s);
        if (sector.empty()) continue;
        for (int i = 0; i < 512; i += 32) {
            if (sector[i] == 0x00) return entries;
            if (sector[i] == 0xE5 || (sector[i+11] & 0x08)) continue;
            
            DirEntry entry;
            char raw_name[9], raw_ext[4];
            std::memcpy(raw_name, &sector[i], 8); raw_name[8] = '\0';
            std::memcpy(raw_ext, &sector[i+8], 3); raw_ext[3] = '\0';

            std::string name(raw_name);
            name.erase(name.find_last_not_of(' ') + 1, std::string::npos);
            std::string ext(raw_ext);
            ext.erase(ext.find_last_not_of(' ') + 1, std::string::npos);

            entry.filename = name + (ext.empty() ? "" : "." + ext);
            entry.start_cluster = sector[i+26] | (sector[i+27] << 8);
            entry.size = sector[i+28] | (sector[i+29] << 8) | (sector[i+30] << 16) | (sector[i+31] << 24);
            entries.push_back(entry);
        }
    }
    return entries;
}

bool Fat12Driver::extract_file(const std::string& filename_on_disk, const std::string& local_dest_path) {
    auto entries = list_root_directory();
    auto it = std::find_if(entries.begin(), entries.end(), [&](const DirEntry& e) {
        return e.filename == filename_on_disk;
    });

    if (it == entries.end()) return false;

    std::ofstream ofs(local_dest_path, std::ios::binary);
    if (!ofs) return false;

    uint16_t current_cluster = it->start_cluster;
    uint32_t bytes_remaining = it->size;

    while (current_cluster >= 0x002 && current_cluster <= 0xFEF) {
        for (int i = 0; i < 2 && bytes_remaining > 0; ++i) {
            auto sector = disk_.get_sector(18 + (current_cluster - 2) * 2 + i);
            uint32_t to_write = std::min((uint32_t)512, bytes_remaining);
            ofs.write((char*)sector.data(), to_write);
            bytes_remaining -= to_write;
        }
        if (bytes_remaining == 0) break;
        current_cluster = get_fat_entry(current_cluster);
    }
    return true;
}

bool Fat12Driver::inject_file(const std::string& local_path, std::string target_name) {
    std::ifstream ifs(local_path, std::ios::binary | std::ios::ate);
    if (!ifs) return false;
    uint32_t file_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(file_size);
    ifs.read((char*)buffer.data(), file_size);

    int entry_sector = -1, entry_offset = -1;
    for (int s = 11; s <= 17 && entry_sector == -1; ++s) {
        auto sector = disk_.get_sector(s);
        for (int i = 0; i < 512; i += 32) {
            if (sector[i] == 0x00 || sector[i] == 0xE5) {
                entry_sector = s; entry_offset = i; break;
            }
        }
    }
    if (entry_sector == -1) return false;

    uint16_t first_cluster = find_free_cluster();
    uint16_t current_cluster = first_cluster;
    uint32_t bytes_remaining = file_size, buf_pos = 0;

    while (bytes_remaining > 0) {
        set_fat_entry(current_cluster, 0xFFF);
        for (int i = 0; i < 2 && bytes_remaining > 0; ++i) {
            auto sector = disk_.get_sector(18 + (current_cluster - 2) * 2 + i);
            uint32_t to_write = std::min((uint32_t)512, bytes_remaining);
            std::memcpy(sector.data(), &buffer[buf_pos], to_write);
            buf_pos += to_write; bytes_remaining -= to_write;
        }
        if (bytes_remaining > 0) {
            uint16_t next = find_free_cluster();
            set_fat_entry(current_cluster, next);
            current_cluster = next;
        }
    }

    auto root_sector = disk_.get_sector(entry_sector);
    uint8_t* entry = &root_sector[entry_offset];
    std::memset(entry, 0x20, 11);
    size_t dot = target_name.find('.');
    std::string base = target_name.substr(0, dot);
    std::string ext = (dot != std::string::npos) ? target_name.substr(dot + 1) : "";
    std::memcpy(entry, base.c_str(), std::min((size_t)8, base.length()));
    std::memcpy(entry + 8, ext.c_str(), std::min((size_t)3, ext.length()));
    entry[11] = 0x00;
    entry[26] = first_cluster & 0xFF; entry[27] = (first_cluster >> 8) & 0xFF;
    entry[28] = file_size & 0xFF; entry[29] = (file_size >> 8) & 0xFF;
    entry[30] = (file_size >> 16) & 0xFF; entry[31] = (file_size >> 24) & 0xFF;
    return true;
}

} // namespace libste
