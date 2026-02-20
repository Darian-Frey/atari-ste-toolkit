#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <string>
#include <arpa/inet.h>

std::string get_ea_mode(uint8_t mode, uint8_t reg) {
    switch (mode) {
        case 0: return "D" + std::to_string(reg);
        case 1: return "A" + std::to_string(reg);
        case 2: return "(A" + std::to_string(reg) + ")";
        case 3: return "(A" + std::to_string(reg) + ")+";
        case 4: return "-(A" + std::to_string(reg) + ")";
        case 7: 
            if (reg == 1) return "(abs.L)";
            if (reg == 0) return "(abs.W)";
            return "#imm";
        default: return "???";
    }
}

void disassemble_68k(const std::vector<uint8_t>& code) {
    size_t pc = 0;
    std::cout << "--- Atari 68k Disassembly ---" << std::endl;

    while (pc < code.size() - 1) {
        // Read 16-bit opcode (Big Endian)
        uint16_t opcode = ntohs(*(uint16_t*)&code[pc]);
        uint32_t current_pc = pc;
        std::string instr = "DC.W";
        std::string args = "";
        uint32_t instr_len = 2;

        // Simple Opcode Matching
        if (opcode == 0x4E71) {
            instr = "NOP";
        } else if (opcode == 0x4E75) {
            instr = "RTS";
        } else if ((opcode & 0xF000) == 0x1000) { // MOVE.B
            instr = "MOVE.B";
            uint8_t src_reg = (opcode >> 0) & 0x7;
            uint8_t src_mode = (opcode >> 3) & 0x7;
            uint8_t dest_reg = (opcode >> 9) & 0x7;
            uint8_t dest_mode = (opcode >> 6) & 0x7;
            args = get_ea_mode(src_mode, src_reg) + ", " + get_ea_mode(dest_mode, dest_reg);
        } else if ((opcode & 0xF1FF) == 0x41F9) { // LEA
            instr = "LEA";
            instr_len = 6;
            if (pc + 5 < code.size()) {
                uint32_t addr = ntohl(*(uint32_t*)&code[pc + 2]);
                args = "$" + std::to_string(addr) + ", A" + std::to_string((opcode >> 9) & 0x7);
            }
        } else {
            // Fallback: output hex word
            std::stringstream ss;
            ss << "$" << std::hex << std::uppercase << opcode;
            args = ss.str();
        }

        // Output formatting
        std::cout << std::hex << std::setw(6) << std::setfill('0') << current_pc << ": ";
        std::cout << std::setw(8) << std::left << instr << args << std::endl;
        
        pc += instr_len;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: st-disasm <binary_file>" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1], std::ios::binary);
    if (!ifs) {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(ifs)), 
                                 std::istreambuf_iterator<char>());

    if (buffer.empty()) return 0;
    disassemble_68k(buffer);

    return 0;
}
