#include "DiskHandler.hpp"
#include "Fat12Driver.hpp"
#include <iostream>

using namespace libste;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: st-inject <disk.st> <local_file> <target_name_on_disk>" << std::endl;
        std::cout << "Example: ./st-inject mydisk.st hello.prg HELLO.PRG" << std::endl;
        return 1;
    }

    std::string disk_path = argv[1];
    std::string local_path = argv[2];
    std::string target_name = argv[3];

    DiskHandler disk;
    if (!disk.load_from_file(disk_path)) {
        std::cerr << "Error: Could not open disk image: " << disk_path << std::endl;
        return 1;
    }

    Fat12Driver fs(disk);
    std::cout << "Injecting " << local_path << " as " << target_name << "..." << std::endl;

    if (fs.inject_file(local_path, target_name)) {
        if (disk.save_to_file(disk_path)) {
            std::cout << "Successfully injected and saved to disk!" << std::endl;
        } else {
            std::cerr << "Error: Could not save changes to disk image." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: Injection failed (disk full or file not found)." << std::endl;
        return 1;
    }

    return 0;
}
