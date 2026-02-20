#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <arpa/inet.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// Helper to convert Atari ST color word to 24-bit RGB
void atari_to_rgb(uint16_t word, uint8_t& r, uint8_t& g, uint8_t& b) {
    // Standard ST 3-bit: bits 8-10 (R), 4-6 (G), 0-2 (B)
    // Multiplied by 36 to scale 0-7 up to ~0-255
    r = ((word >> 8) & 0x07) * 36;
    g = ((word >> 4) & 0x07) * 36;
    b = (word & 0x07) * 36;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: pi1-to-png <input.pi1> <output.png>\n";
        return 1;
    }

    std::ifstream ifs(argv[1], std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Could not open " << argv[1] << "\n";
        return 1;
    }

    // Degas PI1 files start with a resolution word (0 = Low Res)
    uint16_t res;
    ifs.read((char*)&res, 2);

    // Read Palette (16 words / 32 bytes)
    uint16_t raw_palette[16];
    ifs.read((char*)raw_palette, 32);
    uint8_t palette_rgb[16][3];
    for(int i=0; i<16; ++i) {
        atari_to_rgb(ntohs(raw_palette[i]), palette_rgb[i][0], palette_rgb[i][1], palette_rgb[i][2]);
    }

    // Read Image Data (32,000 bytes)
    std::vector<uint8_t> screen(32000);
    ifs.read((char*)screen.data(), 32000);

    // De-planarize into RGBA buffer (320x200)
    std::vector<uint8_t> rgba(320 * 200 * 4);
    for (int y = 0; y < 200; ++y) {
        for (int x_chunk = 0; x_chunk < 20; ++x_chunk) { // 20 chunks of 16 pixels
            // Each chunk is 4 words (8 bytes) representing 16 pixels
            size_t offset = (y * 160) + (x_chunk * 8);
            uint16_t p0 = ntohs(*(uint16_t*)&screen[offset]);
            uint16_t p1 = ntohs(*(uint16_t*)&screen[offset + 2]);
            uint16_t p2 = ntohs(*(uint16_t*)&screen[offset + 4]);
            uint16_t p3 = ntohs(*(uint16_t*)&screen[offset + 6]);

            for (int p = 0; p < 16; ++p) {
                int bit = 15 - p;
                int color_idx = ((p0 >> bit) & 1) | 
                                (((p1 >> bit) & 1) << 1) | 
                                (((p2 >> bit) & 1) << 2) | 
                                (((p3 >> bit) & 1) << 3);
                
                int px = (x_chunk * 16) + p;
                int rgba_idx = (y * 320 + px) * 4;
                rgba[rgba_idx]     = palette_rgb[color_idx][0];
                rgba[rgba_idx + 1] = palette_rgb[color_idx][1];
                rgba[rgba_idx + 2] = palette_rgb[color_idx][2];
                rgba[rgba_idx + 3] = 255; // Alpha
            }
        }
    }

    if (stbi_write_png(argv[2], 320, 200, 4, rgba.data(), 320 * 4)) {
        std::cout << "Successfully recovered image to " << argv[2] << "\n";
    } else {
        std::cerr << "Error writing PNG file.\n";
        return 1;
    }

    return 0;
}
