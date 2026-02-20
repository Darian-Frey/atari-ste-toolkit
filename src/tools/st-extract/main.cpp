#include "DiskHandler.hpp"
#include "Fat12Driver.hpp"
#include <iostream>

using namespace libste;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: st-extract <disk.st> <filename_on_disk> <local_dest_path>" << std::endl;
        std::cout << "Example: ./st-extract mydisk.st TEST.TXT restored.txt" << std::endl;
        return 1;
    }

    DiskHandler disk;
    if (!disk.load_from_file(argv[1])) {
        std::cerr << "Error: Could not open disk image." << std::endl;
        return 1;
    }

    Fat12Driver fs(disk);
    if (fs.extract_file(argv[2], argv[3])) {
        std::cout << "Successfully extracted " << argv[2] << " to " << argv[3] << std::endl;
    } else {
        std::cerr << "Error: Extraction failed. Is the filename correct (e.g., TEST.TXT)?" << std::endl;
        return 1;
    }
    return 0;
}
