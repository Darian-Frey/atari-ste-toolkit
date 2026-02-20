#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <arpa/inet.h> // for htons (Big Endian conversion)

void chunky_to_planar(const std::vector<uint8_t>& chunky, std::vector<uint8_t>& planar) {
    // Atari Low Res: 320x200, 4 planes (16 colors)
    // 16 pixels are grouped into 4 words (8 bytes)
    for (size_t i = 0; i < chunky.size(); i += 16) {
        uint16_t planes[4] = {0, 0, 0, 0};

        for (int p = 0; p < 16; ++p) {
            uint8_t color_index = chunky[i + p] & 0x0F;
            for (int b = 0; b < 4; ++b) {
                if (color_index & (1 << b)) {
                    // Atari bits are high-to-low within the word
                    planes[b] |= (1 << (15 - p));
                }
            }
        }

        // Write the 4 words in order (Plane 0, 1, 2, 3)
        for (int b = 0; b < 4; ++b) {
            uint16_t be_word = htons(planes[b]); // Atari is Big Endian
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&be_word);
            planar.push_back(ptr[0]);
            planar.push_back(ptr[1]);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: st-planar <input.chunky> <output.bin>\n";
        return 1;
    }

    std::ifstream ifs(argv[1], std::ios::binary);
    std::vector<uint8_t> chunky((std::istreambuf_iterator<char>(ifs)), 
                                 std::istreambuf_iterator<char>());

    if (chunky.size() % 16 != 0) {
        std::cerr << "Warning: Input size not multiple of 16. Padding with zeros.\n";
        while (chunky.size() % 16 != 0) chunky.push_back(0);
    }

    std::vector<uint8_t> planar;
    chunky_to_planar(chunky, planar);

    std::ofstream ofs(argv[2], std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(planar.data()), planar.size());

    std::cout << "Converted " << chunky.size() << " chunky pixels to " 
              << planar.size() << " bytes of planar data.\n";

    return 0;
}
