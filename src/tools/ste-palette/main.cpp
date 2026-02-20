#include <iostream>
#include <string>
#include <iomanip>

// Helper to convert a 0-15 value to the weird Atari STE bit order:
// Atari STE bit 3 is actually moved to the position of bit 0.
// Expected order in nibble: [Bit 3] [Bit 0] [Bit 1] [Bit 2]
uint8_t to_ste_nibble(uint8_t value) {
    value &= 0x0F;
    uint8_t bit3 = (value >> 3) & 0x01;
    uint8_t bits012 = value & 0x07;
    return (bits012 << 1) | bit3;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ste-palette <hex_color>\n";
        std::cout << "Example: ste-palette #FF8800\n";
        return 1;
    }

    std::string hex = argv[1];
    if (hex[0] == '#') hex.erase(0, 1);

    if (hex.length() != 6) {
        std::cerr << "Error: Provide a 6-digit hex color (RRGGBB).\n";
        return 1;
    }

    // Extract 8-bit components
    uint8_t r8 = std::stoi(hex.substr(0, 2), nullptr, 16);
    uint8_t g8 = std::stoi(hex.substr(2, 2), nullptr, 16);
    uint8_t b8 = std::stoi(hex.substr(4, 2), nullptr, 16);

    // Downsample to 4-bit (0-15)
    uint8_t r4 = r8 >> 4;
    uint8_t g4 = g8 >> 4;
    uint8_t b4 = b8 >> 4;

    // Convert to STE nibble format
    uint16_t ste_r = to_ste_nibble(r4);
    uint16_t ste_g = to_ste_nibble(g4);
    uint16_t ste_b = to_ste_nibble(b4);

    // Combine into a 16-bit word (Atari format: 0RRR 0GGG 0BBB)
    // Note: The high nibble is usually 0 in ST/STE palettes.
    uint16_t palette_word = (ste_r << 8) | (ste_g << 4) | ste_b;

    std::cout << "Input Hex: #" << hex << "\n";
    std::cout << "STE 4-bit: R:" << (int)r4 << " G:" << (int)g4 << " B:" << (int)b4 << "\n";
    std::cout << "Atari Word: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << palette_word << "\n";

    return 0;
}
