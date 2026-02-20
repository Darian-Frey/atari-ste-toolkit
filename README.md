# 🕹️ ATARI STE TOOLKIT v1.0 🕹️

```text
    __________  __________  ____  __ __ __________
   / ____/ __ \/ ____/ __ \/ __ \/ //_// ____/_  /
  / /_  / / / / / __/ /_/ / / / / ,<  / __/   / / 
 / __/ / /_/ / /_/ / _, _/ /_/ / /| |/ /___  / /  
/_/    \____/\____/_/ |_|\____/_/ |_/_____/ /_/   
                                                  
        >>> THE 16-BIT DEVELOPER SUITE <<<
```

---

## [ STATUS: BOOTING PROTOCOL... ]

A comprehensive C++20 developer and recovery suite for the **Atari ST/STE**. This toolkit bridges the gap between modern Linux environments and 16-bit Motorola 68000 hardware.

---

## 🛠️ THE UTILITIES

### 💾 STORAGE & FILESYSTEM
* **st-mkdisk** :: Generate 720KB (DD) .ST disk images.
* **st-check** :: Validate TOS boot sector checksums.
* **st-dir** :: List contents of the FAT12 root directory.
* **st-inject** :: Push local files into the Atari disk image.
* **st-extract** :: Pull legacy data back to the modern world.

### 🎨 VIDEO & PALETTE
* **ste-palette** :: Convert RGB Hex to 12-bit STE hardware words.
* **st-planar** :: Transform chunky pixels to 4-plane bitplanes.
* **pi1-to-png** :: Recover DEGAS Elite (.PI1) art as PNG.

### 🔊 AUDIO SAMPLES
* **ste-dma-snd** :: Convert 8-bit unsigned to STE Signed PCM.
* **ste-snd-wav** :: Recover Atari DMA audio to RIFF/WAV format.

### 🔍 CODE & REVERSING
* **st-bin2rsx** :: Binary-to-Header resource converter.
* **st-disasm** :: Motorola 68000 instruction disassembler.

---

## 🚀 ASSEMBLY INSTRUCTIONS

Requires a C++20 compliant compiler (GCC 11+ / Clang 13+).

```bash
mkdir build && cd build
cmake ..
make
```

---

## 🕹️ OPERATION EXAMPLES

**> Injecting a Demo:**
```bash
./st-inject demo.st INTRO.PRG AUTOEXEC.TOS
```

**> Decoding STE Colors:**
```bash
./ste-palette "#FF00FF"
# ATARI WORD: 0x0F0F
```

---

## 📟 HARDWARE SPECS
* **DISK**: 80 Tracks, 9 Sectors, 2 Sides (720KB).
* **CPU**: Motorola 68000 (CISC).
* **AUDIO**: 8-bit Signed PCM (STE DMA).
* **GRAPHICS**: 320x200, 16 colors from 4096 (STE).

---
**[ END OF TRANSMISSION ]**
