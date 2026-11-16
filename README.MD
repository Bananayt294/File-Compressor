# Huffman Compressor

A Windows file compression and decompression utility written in C++20 using Huffman coding. It handles individual files and integrates directly into Windows File Explorer via right-click context menus.

> **Work in Progress:** Folder compression and extraction are still being built. Right now, running `compressor.exe compress "<directory>"` just lists the folder's contents without packing them. Single-file compression and extraction work as expected.

---

## Quick Usage

* **Compress a file:** Right-click any supported file in File Explorer and select **Compress with Huffman Compressor**. This drops a `.huff` archive right next to the original file (overwriting any existing `.huff` file of the same name).
* **Extract an archive:** Right-click a `.huff` file and choose **Extract with Huffman Compressor**, or just double-click it. The original file is restored in the same folder. If a file with that name is already there, a Windows **Save As** dialog pops up so you can pick a new spot.

### Current Limitations
* **Minimum entropy:** Input files must contain at least two distinct byte values. Empty files or files made up of a single repeated byte aren't supported yet.
* **File size:** Uses 32-bit signed counters, meaning files close to or larger than 2 GiB won't work.
* **Expansion:** Depending on the input, storing the Huffman tree metadata can occasionally make the `.huff` file slightly larger than the original.

---

## Features & Architecture

### Core Engine
* **Lossless Compression:** Builds a custom Min Heap to construct the Huffman tree, generates variable-length binary codes from byte frequencies, and packs bits into bytes before writing to disk.
* **Metadata & Restoration:** Embeds the original file size, unique byte count, and tree structure inside the `.huff` file, validating headers before decoding the bitstream back into the original file.
* **Overwrite Protection:** Uses Windows APIs (`GetSaveFileNameW()`) to prompt on name collisions during extraction, backed by exclusive file creation flags (`_O_EXCL`) at the CRT level to prevent race-condition overwrites.

### Windows Integration & Installer
Packaged with **Inno Setup** (`HuffmannCompressor.iss`), the installer runs without administrator privileges and installs to `%LOCALAPPDATA%\Programs\Huffman Compressor`. It handles:
* Explorer context-menu entries for compressing and extracting.
* `.huff` file associations, double-click extraction, and a custom archive icon.
* Start Menu shortcuts, an optional desktop shortcut, and a standard Windows uninstaller.

### Tech Stack

| Component | Role |
| :--- | :--- |
| **C++20** | Core compression/decompression logic |
| **Huffman Coding & Min Heap** | Lossless encoding and tree construction |
| **Windows CRT & Win32 API** | Low-level binary I/O (`_O_EXCL`) and native **Save As** dialogs (`comdlg32`) |
| **`std::filesystem`** | Path handling and directory traversal |
| **MSYS2 UCRT64 / G++** | Compilation toolchain |
| **Inno Setup** | Windows installer and uninstaller packaging |

---

## How It Works

**Compression**
1. Opens the target file in binary mode and counts byte frequencies.
2. Feeds frequencies into a Min Heap and merges the two lowest-frequency nodes repeatedly until one root tree remains.
3. Walks the tree to assign binary codes to each unique byte.
4. Writes a header containing the original file size, unique byte count, and tree metadata.
5. Encodes the input stream, packs the bits into bytes, and flushes them to the `.huff` file.

**Decompression**
1. Reads and validates the `.huff` header (original size and unique byte count).
2. Rebuilds the Huffman tree from the stored metadata.
3. Reads the compressed bitstream bit-by-bit, traversing the tree to write decoded bytes out to disk until the original file size is reached.

---

## Installation & CLI Usage

### Installing a Release
1. Grab `HuffmanCompressorSetup.exe` from the latest GitHub release and run it.
2. Use the right-click context menus in Explorer, or call the executable directly from PowerShell:

```powershell
# Compress
& "$env:LOCALAPPDATA\Programs\Huffman Compressor\compressor.exe" compress "C:\path\to\file.txt"

# Extract
& "$env:LOCALAPPDATA\Programs\Huffman Compressor\compressor.exe" extract "C:\path\to\file.txt.huff"