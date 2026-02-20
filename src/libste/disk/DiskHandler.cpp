#include "DiskHandler.hpp"
#include <fstream>
#include <numeric>

namespace libste {

DiskHandler::DiskHandler() {}

bool DiskHandler::create_blank(size_t size) {
    // 0xE5 is the standard "empty" byte for floppy formatting
    data_.assign(size, 0xE5);
    return true;
}

bool DiskHandler::load_from_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    data_.resize(static_cast<size_t>(size));
    return file.read(reinterpret_cast<char*>(data_.data()), size) ? true : false;
}

bool DiskHandler::save_to_file(const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    file.write(reinterpret_cast<const char*>(data_.data()), data_.size());
    return file.good();
}

std::span<uint8_t> DiskHandler::get_sector(size_t sector_index) {
    size_t offset = sector_index * SECTOR_SIZE;
    if (offset + SECTOR_SIZE > data_.size()) {
        return {}; // Out of bounds
    }
    return std::span<uint8_t>(&data_[offset], SECTOR_SIZE);
}

void DiskHandler::apply_tos_checksum() {
    if (data_.size() < SECTOR_SIZE) return;

    uint16_t sum = 0;
    // Sum the first 510 bytes as 16-bit Big-Endian words
    for (size_t i = 0; i < 510; i += 2) {
        sum += (data_[i] << 8) | data_[i + 1];
    }

    // Atari TOS check: The sum of the whole sector (as words) must be 0x1234
    uint16_t diff = 0x1234 - sum;
    data_[510] = (diff >> 8) & 0xFF;
    data_[511] = diff & 0xFF;
}

bool DiskHandler::verify_tos_checksum() const {
    if (data_.size() < SECTOR_SIZE) return false;
    uint16_t sum = 0;
    for (size_t i = 0; i < 512; i += 2) {
        sum += (data_[i] << 8) | data_[i + 1];
    }
    return sum == 0x1234;
}

} // namespace libste
