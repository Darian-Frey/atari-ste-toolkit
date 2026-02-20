# Atari STE Toolkit 🕹️ v1.0

A comprehensive C++20 developer and recovery suite for the Atari ST/STE. This toolkit bridges the gap between modern Linux environments and 16-bit hardware, offering tools for filesystem management, media conversion, and 68k analysis.

---

## 🛠️ Tool Inventory

### 💾 Filesystem & Disk Management
* **st-mkdisk**: Create a standard 720KB (DD) bootable disk image.
* **st-check**: Verify the TOS boot sector checksum.
* **st-dir**: List files on a .ST image (FAT12).
* **st-inject**: Inject local files into a disk image.
* **st-extract**: Recover files from a disk image back to Linux.

### 🎨 Graphics & Palette
* **ste-palette**: Convert hex colors to Atari 12-bit (STE) hardware color words.
* **st-planar**: Convert "chunky" pixels to Atari 4-plane bitplane data.
* **pi1-to-png**: Recover DEGAS Elite (.PI1) images into modern PNGs.

### 🔊 Digital Audio
* **ste-dma-snd**: Convert raw 8-bit unsigned audio to STE-ready signed DMA samples.
* **ste-snd-wav**: Wrap STE signed samples into a standard RIFF/WAV header.

### 🔍 Development & Reversing
* **st-bin2rsx**: Convert any binary data into a C/C++ header array for easy inclusion.
* **st-disasm**: A lightweight Motorola 68000 disassembler for binary analysis.

---

## 🚀 Building the Toolkit
Requires a C++20 compliant compiler (GCC 11+ or Clang 13+), CMake, and `wget` for dependency fetching.

```bash
mkdir build && cd build
cmake ..
make
```

---

## 📖 Quick Start Examples

**Create a disk and inject a program:**
```bash
./st-mkdisk my_game.st
./st-inject my_game.st local_code.tos AUTOEXEC.TOS
```

**Recover an old Degas image:**
```bash
./pi1-to-png LEGACY.PI1 recovered.png
```

**Convert a color for the STE palette:**
```bash
./ste-palette "#FF8800"
# Output: 0x0F10
```

---

## 🏗️ Technical Specifications
* **Architecture:** C++20
* **Disk Format:** 720KB, 80 tracks, 9 sectors per track, 2 sides.
* **Filesystem:** FAT12 with 2 sectors per cluster.
* **Audio Rates:** Supports 6.25kHz, 12.5kHz, 25kHz, and 50kHz STE DMA rates.
