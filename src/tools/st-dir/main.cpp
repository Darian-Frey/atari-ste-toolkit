#include "DiskHandler.hpp"
#include "Fat12Driver.hpp"
#include <iostream>
#include <iomanip>

using namespace libste;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: st-dir <filename.st>" << std::endl;
        return 1;
    }

    DiskHandler disk;
    if (!disk.load_from_file(argv[1])) {
        std::cerr << "Could not open disk image: " << argv[1] << std::endl;
        return 1;
    }

    Fat12Driver fs(disk);
    auto files = fs.list_root_directory();

    std::cout << "Directory Listing for " << argv[1] << ":" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    
    if (files.empty()) {
        std::cout << "(Disk is empty)" << std::endl;
    } else {
        for (const auto& f : files) {
            std::cout << std::left << std::setw(15) << f.filename 
                      << " | " << f.size << " bytes" << std::endl;
        }
    }

    return 0;
}
