#pragma once
#include "DiskHandler.hpp"
#include <vector>
#include <string>
#include <cstdint>

namespace libste {

struct DirEntry {
    std::string filename;
    uint32_t size;
    uint16_t start_cluster;
    uint8_t attributes;
};

class Fat12Driver {
public:
    Fat12Driver(DiskHandler& disk);
    std::vector<DirEntry> list_root_directory();
    bool inject_file(const std::string& local_path, std::string target_name);
    bool extract_file(const std::string& filename_on_disk, const std::string& local_dest_path);

private:
    DiskHandler& disk_;
    uint16_t get_fat_entry(uint16_t cluster);
    void set_fat_entry(uint16_t cluster, uint16_t value);
    uint16_t find_free_cluster();
};

} // namespace libste
