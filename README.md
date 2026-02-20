# Atari STE Toolkit 🕹️

A modern C++20 suite of tools for managing Atari ST/STE floppy disk images (.st). This toolkit allows you to create, verify, and inject files into disk images compatible with real hardware and emulators like Hatari or Steem.

---

## 🛠️ Features
* **st-mkdisk**: Create a standard 720KB (Double Sided/Double Density) bootable disk image.
* **st-check**: Verify the boot sector checksum (TOS compatibility).
* **st-dir**: List the contents of the root directory (FAT12).
* **st-inject**: Inject local files into the disk image.

---

## 🚀 Building the Toolkit
Requires a C++20 compliant compiler and CMake 3.16+.

```bash
mkdir build && cd build
cmake ..
make
```

---

## 📖 Usage Guide

### 1. Create a New Disk
Creates a blank 720KB disk image with a valid Atari boot sector.
```bash
./st-mkdisk my_disk.st
```

### 2. Check Disk Integrity
Verifies the TOS executable checksum in the boot sector.
```bash
./st-check my_disk.st
```

### 3. Inject a File
Copies a local file into the disk image's root directory.
**Note:** Use 8.3 filename format for target names.
```bash
./st-inject my_disk.st local_program.tos MYPROG.TOS
```

### 4. List Directory
Displays the files currently stored on the disk image.
```bash
./st-dir my_disk.st
```

---

## 🏗️ Project Structure
* `include/`: Header files for `libste`.
* `src/libste/`: Core logic for disk I/O and FAT12 filesystem handling.
* `src/tools/`: CLI implementations for the toolkit utilities.

---

## ⚠️ Technical Notes
* **Filesystem:** Supports FAT12 with 2 sectors per cluster (720KB standard).
* **Alignment:** Ensure filenames provided to `st-inject` follow the 8.3 uppercase standard for best compatibility with TOS.
