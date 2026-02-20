#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

// RIFF/WAV Header Structure
#pragma pack(push, 1)
struct WavHeader {
    char riff[4] = {'R','I','F','F'};
    uint32_t overall_size;
    char wave[4] = {'W','A','V','E'};
    char fmt_chunk_marker[4] = {'f','m','t',' '};
    uint32_t length_of_fmt = 16;
    uint16_t format_type = 1; // PCM
    uint16_t channels = 1;    // Mono
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align = 1;
    uint16_t bits_per_sample = 8;
    char data_chunk_marker[4] = {'d','a','t','a'};
    uint32_t data_size;
};
#pragma pack(pop)

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: ste-snd-wav <input.snd> <output.wav> <rate>\n";
        std::cout << "Common STE Rates: 6258, 12517, 25033, 50066\n";
        return 1;
    }

    std::ifstream ifs(argv[1], std::ios::binary | std::ios::ate);
    if (!ifs) {
        std::cerr << "Error: Could not open input file.\n";
        return 1;
    }
    
    uint32_t data_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(data_size);
    ifs.read((char*)buffer.data(), data_size);

    // Atari STE uses SIGNED 8-bit (-128 to 127). 
    // Standard WAV uses UNSIGNED 8-bit (0 to 255).
    // We must shift the data back for modern players.
    for(auto &sample : buffer) {
        sample = static_cast<uint8_t>(static_cast<int8_t>(sample) + 128);
    }

    uint32_t rate = std::stoi(argv[3]);
    WavHeader header;
    header.sample_rate = rate;
    header.byte_rate = rate;
    header.data_size = data_size;
    header.overall_size = data_size + sizeof(WavHeader) - 8;

    std::ofstream ofs(argv[2], std::ios::binary);
    ofs.write((char*)&header, sizeof(WavHeader));
    ofs.write((char*)buffer.data(), data_size);

    std::cout << "Successfully converted STE audio to " << argv[2] << " (" << rate << "Hz)\n";
    return 0;
}
