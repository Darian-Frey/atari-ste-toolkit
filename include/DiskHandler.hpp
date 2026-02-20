#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <span>

namespace libste {

class DiskHandler {
public:
    static constexpr size_t SECTOR_SIZE = 512;
    static constexpr size_t DEFAULT_720K_SIZE = 737280;

    DiskHandler();
    
    // Disk Image Lifecycle
    bool create_blank(size_t size = DEFAULT_720K_SIZE);
    bool load_from_file(const std::string& path);
    bool save_to_file(const std::string& path);

    // Raw Sector Access
    std::span<uint8_t> get_sector(size_t sector_index);
    
    // Atari Specifics
    void apply_tos_checksum();
    bool verify_tos_checksum() const;

    size_t get_total_size() const { return data_.size(); }

private:
    std::vector<uint8_t> data_;
};

} // namespace libste
