#include "DiskHandler.hpp"
#include <iostream>
using namespace libste;
int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    DiskHandler disk;
    if (!disk.load_from_file(argv[1])) return 1;
    std::cout << "Disk: " << argv[1] << " [Check PASSED]" << std::endl;
    return 0;
}
