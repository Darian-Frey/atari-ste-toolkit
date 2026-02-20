#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <string>

// The Atari STE DMA chip supports 4 specific frequencies:
// 0: 6258 Hz
// 1: 12517 Hz
// 2: 25033 Hz
// 3: 50066 Hz

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ste-dma-snd <input_raw_unsigned> <output_ste_signed>\n";
        std::cout << "Note: Input should be 8-bit raw mono data.\n";
        return 1;
    }

    std::ifstream ifs(argv[1], std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::cerr << "Error: Could not open input file.\n";
        return 1;
    }

    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Error reading file.\n";
        return 1;
    }

    // Convert Unsigned 8-bit (0 to 255) to Signed 8-bit (-128 to 127)
    // The STE DMA hardware expects signed data.
    // Logic: subtract 128 from the unsigned value.
    for (size_t i = 0; i < buffer.size(); ++i) {
        int signed_val = static_cast<int>(buffer[i]) - 128;
        buffer[i] = static_cast<int8_t>(signed_val);
    }

    std::ofstream ofs(argv[2], std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    std::cout << "Converted " << size << " samples to STE signed format.\n";
    std::cout << "Hardware Tip: Set \$FF8901 to enable DMA playback.\n";

    return 0;
}
