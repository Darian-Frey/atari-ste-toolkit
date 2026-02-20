#include "Fat12Driver.hpp"
#include <cstring>
#include <fstream>
#include <algorithm>

namespace libste {

Fat12Driver::Fat12Driver(DiskHandler& disk) : disk_(disk) {}

// Helper to read 12-bit FAT entry
uint16_t Fat12Driver::get_fat_entry(uint16_t cluster) {
    auto fat = disk_.get_sector(1); // FAT starts at Sector 1
    size_t offset = (cluster * 3) / 2;
    uint16_t value;
    if (cluster % 2 == 0) {
        value = fat[offset] | ((fat[offset + 1] & 0x0F) << 8);
    } else {
        value = (fat[offset] >> 4) | (fat[offset + 1] << 4);
    }
    return value;
}

// Helper to write 12-bit FAT entry
void Fat12Driver::set_fat_entry(uint16_t cluster, uint16_t value) {
    auto fat = disk_.get_sector(1); 
    size_t offset = (cluster * 3) / 2;
    if (cluster % 2 == 0) {
        fat[offset] = value & 0xFF;
        fat[offset + 1] = (fat[offset + 1] & 0xF0) | ((value >> 8) & 0x0F);
    } else {
        fat[offset] = (fat[offset] & 0x0F) | ((value << 4) & 0xF0);
        fat[offset + 1] = (value >> 4) & 0xFF;
    }
    // Note: In a real driver, you'd mirror this to the second FAT at Sector 6
}

uint16_t Fat12Driver::find_free_cluster() {
    for (uint16_t c = 2; c < 1440; ++c) {
        if (get_fat_entry(c) == 0x000) return c;
    }
    return 0;
}

bool Fat12Driver::inject_file(const std::string& local_path, std::string target_name) {
    std::ifstream ifs(local_path, std::ios::binary | std::ios::ate);
    if (!ifs) return false;
    uint32_t file_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(file_size);
    ifs.read((char*)buffer.data(), file_size);

    // 1. Find empty Root Dir entry (Sectors 11-17)
    int entry_sector = -1;
    int entry_offset = -1;
    for (int s = 11; s <= 17 && entry_sector == -1; ++s) {
        auto sector = disk_.get_sector(s);
        for (int i = 0; i < 512; i += 32) {
            if (sector[i] == 0x00 || sector[i] == 0xE5) {
                entry_sector = s;
                entry_offset = i;
                break;
            }
        }
    }

    if (entry_sector == -1) return false;

    // 2. Allocate clusters and write data
    uint16_t first_cluster = find_free_cluster();
    uint16_t current_cluster = first_cluster;
    uint32_t bytes_remaining = file_size;
    uint32_t buf_pos = 0;

    while (bytes_remaining > 0) {
        set_fat_entry(current_cluster, 0xFFF); // Mark as end for now
        
        // Cluster to Sector mapping: Data starts at Sector 18. Cluster 2 = Sector 18, 19...
        // Assuming 2 sectors per cluster for 720KB
        for (int i = 0; i < 2 && bytes_remaining > 0; ++i) {
            auto sector = disk_.get_sector(18 + (current_cluster - 2) * 2 + i);
            uint32_t to_write = std::min((uint32_t)512, bytes_remaining);
            std::memcpy(sector.data(), &buffer[buf_pos], to_write);
            buf_pos += to_write;
            bytes_remaining -= to_write;
        }

        if (bytes_remaining > 0) {
            uint16_t next = find_free_cluster();
            set_fat_entry(current_cluster, next);
            current_cluster = next;
        }
    }

    // 3. Write Directory Entry
    auto root_sector = disk_.get_sector(entry_sector);
    uint8_t* entry = &root_sector[entry_offset];
    std::memset(entry, 0x20, 11); // Fill name with spaces
    
    // Simple 8.3 conversion (Assumes target_name is valid 8.3)
    size_t dot = target_name.find('.');
    std::string base = target_name.substr(0, dot);
    std::string ext = (dot != std::string::npos) ? target_name.substr(dot + 1) : "";
    std::memcpy(entry, base.c_str(), std::min((size_t)8, base.length()));
    std::memcpy(entry + 8, ext.c_str(), std::min((size_t)3, ext.length()));

    entry[11] = 0x00; // Normal file attribute
    entry[26] = first_cluster & 0xFF;
    entry[27] = (first_cluster >> 8) & 0xFF;
    entry[28] = file_size & 0xFF;
    entry[29] = (file_size >> 8) & 0xFF;
    entry[30] = (file_size >> 16) & 0xFF;
    entry[31] = (file_size >> 24) & 0xFF;

    return true;
}

std::vector<DirEntry> Fat12Driver::list_root_directory() {
    std::vector<DirEntry> entries;
    for (int s = 11; s <= 17; ++s) {
        auto sector = disk_.get_sector(s);
        if (sector.empty()) continue;
        for (int i = 0; i < 512; i += 32) {
            if (sector[i] == 0x00) return entries;
            if (sector[i] == 0xE5 || (sector[i+11] & 0x08)) continue; // Skip empty/volume labels
            
            DirEntry entry;
            char name[9], ext[4];
            std::memcpy(name, &sector[i], 8); name[8] = '\0';
            std::memcpy(ext, &sector[i+8], 3); ext[3] = '\0';
            entry.filename = std::string(name) + "." + std::string(ext);
            entry.size = sector[i+28] | (sector[i+29] << 8) | (sector[i+30] << 16) | (sector[i+31] << 24);
            entries.push_back(entry);
        }
    }
    return entries;
}

}
