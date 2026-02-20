#include "DiskHandler.hpp"
#include <iostream>
#include <string>

using namespace libste;

void initialize_bpb(DiskHandler& disk) {
    auto sector = disk.get_sector(0);
    if (sector.empty()) return;

    // Standard 720KB (Double Sided, 9 Sectors, 80 Tracks) BPB
    sector[0x0B] = 0x00; sector[0x0C] = 0x02; // Sector size: 512
    sector[0x0D] = 0x02;                   // Sectors per cluster: 2
    sector[0x0E] = 0x01; sector[0x0F] = 0x00; // Reserved sectors: 1
    sector[0x10] = 0x02;                   // Number of FATs: 2
    sector[0x11] = 0x70; sector[0x12] = 0x00; // Max directory entries: 112
    sector[0x13] = 0xA0; sector[0x14] = 0x05; // Total sectors: 1440
    sector[0x15] = 0xF9;                   // Media descriptor: 3.5" DS
    sector[0x16] = 0x05; sector[0x17] = 0x00; // Sectors per FAT: 5
    sector[0x18] = 0x09; sector[0x19] = 0x00; // Sectors per track: 9
    sector[0x1A] = 0x02; sector[0x1B] = 0x00; // Number of sides: 2

    // Apply the Atari-specific boot checksum
    disk.apply_tos_checksum();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: st-mkdisk <filename.st>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    DiskHandler disk;

    std::cout << "Generating 720KB Atari Disk Image: " << filename << "..." << std::endl;

    if (!disk.create_blank()) {
        std::cerr << "Failed to allocate memory for disk image." << std::endl;
        return 1;
    }

    initialize_bpb(disk);

    if (disk.save_to_file(filename)) {
        std::cout << "Success! Validated Atari Boot Checksum: " 
                  << (disk.verify_tos_checksum() ? "PASSED" : "FAILED") << std::endl;
    } else {
        std::cerr << "Error: Could not save file." << std::endl;
        return 1;
    }

    return 0;
}
