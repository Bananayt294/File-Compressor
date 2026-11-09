# Huffman Compressor

A Windows-based text-file compression and decompression utility written in C++ using Huffman Coding. The project implements a custom binary archive format, provides command-line functionality, and integrates directly with Windows File Explorer through right-click context menus.

The long-term goal is to develop a standalone Windows compression utility with support for individual files, directories, and recursive folder structures.

## Features

### Huffman Compression

- Implements Huffman Coding for lossless data compression.
- Builds a Huffman tree using a custom Min Heap.
- Generates variable-length binary codes based on character frequencies.
- Packs encoded bits into bytes for storage.
- Stores the metadata required to reconstruct the Huffman tree during decompression.
- Supports binary file I/O using Windows file APIs.

### Decompression

- Reconstructs the Huffman tree from stored metadata.
- Decodes the compressed bitstream.
- Reconstructs the original file contents.
- Validates archive metadata before attempting decompression.
- Handles single-character edge cases.

### Archive Management

- Uses a custom `.huff` archive extension.
- Generates compressed archive files from input files.
- Extracts files from existing archives.
- Prevents accidental overwriting of existing files.
- Opens a native Windows Save As dialog when an extracted filename already exists, allowing the user to choose a different destination filename.

### Windows Integration

The application integrates with Windows File Explorer through its installer.

- **Compress with Huffman Compressor:** Right-click any text file to compress it.
- **Extract with Huffman Compressor:** Right-click a `.huff` archive to extract it.
- Double-click `.huff` archives to initiate extraction.
- Custom `.huff` archive icon.
- Start Menu shortcuts.
- Optional desktop shortcut.
- Automatic uninstaller.

### Installer

The application is packaged using Inno Setup.

- Installs under `%LOCALAPPDATA%\Programs\Huffman Compressor`.
- Registers file associations and Windows context-menu commands.
- Installs the custom archive icon.
- Creates an uninstaller and registers the application with Windows.
- Supports installation without administrator privileges.

## Technology Stack

| Technology | Purpose |
|---|---|
| C++20 | Core implementation |
| Huffman Coding | Lossless compression algorithm |
| Min Heap | Huffman tree construction |
| Windows CRT | Low-level binary file operations |
| Windows API | Native Save As dialog |
| `std::filesystem` | Directory traversal and path management |
| Inno Setup | Windows installer and uninstaller |
| MSYS2 UCRT64 / G++ | Compilation |

## How It Works

### 1. Compression

The compression process follows these steps:

1. Opens the input file in binary mode.
2. Reads the file and calculates the frequency of each character.
3. Constructs a Min Heap using the character frequencies.
4. Builds the Huffman tree by repeatedly extracting the two nodes with the lowest frequencies.
5. Generates Huffman codes from the resulting tree.
6. Stores the original file size, unique character count, and Huffman metadata.
7. Encodes the input using the generated codes.
8. Packs the encoded bits into bytes and writes them to the archive.

### 2. Decompression

The decompression process reverses the encoding process:

1. Opens the `.huff` archive.
2. Reads the original file size and unique character count.
3. Reconstructs the Huffman tree using the stored metadata.
4. Reads the compressed bitstream.
5. Traverses the Huffman tree to decode the original characters.
6. Writes the reconstructed contents to the output file.

### 3. Preventing File Overwrites

During extraction, the application checks whether the intended output filename already exists.

If it does, the application opens the native Windows Save As dialog using `GetSaveFileNameW()`.

The user can select a different filename or cancel the extraction.

File creation uses exclusive creation flags (`_O_EXCL`) to prevent an existing file from being overwritten, including in the event of a race between checking the filename and creating it.

## Building From Source

### Requirements

- Windows
- C++20-compatible compiler
- MSYS2 UCRT64 / MinGW-w64 G++
- Windows Common Dialog library (`comdlg32`)

### Compilation

From the source directory:

```bash
g++ -std=c++20 -O2 compressor.cpp -o compressor.exe -lcomdlg32
```

### Command-Line Usage

Compress a file:

```powershell
.\compressor.exe compress "C:\path\to\file.txt"
```

Extract an archive:

```powershell
.\compressor.exe extract "C:\path\to\file.txt.huff"
```

The application also provides an interactive console menu when launched without command-line arguments.

## Windows Installation

The installer is built using Inno Setup.

1. Open `HuffmanCompressor.iss` in Inno Setup.
2. Ensure the latest compiled application executable is in the expected source directory.
3. Compile the installer using `F9`.
4. Locate the generated setup executable in the output directory.

The installer is generated as:

```text
Output/
└── HuffmanCompressorSetup.exe
```

The Inno Setup script handles application installation, Windows registry integration, file associations, shortcuts, and uninstallation.

## Folder Compression and Extraction — In Development

The next major feature is support for compressing and extracting entire directories while preserving their original directory structures.

The intended behaviour is:

**Input directory:**

```text
MyFolder/
│
├── document.txt
├── image.png
│
└── Projects/
    ├── main.cpp
    └── config.txt
```

**Expected archive:**

```text
MyFolder.huff
```

**Expected extracted structure:**

```text
MyFolder/
│
├── document.txt
├── image.png
│
└── Projects/
    ├── main.cpp
    └── config.txt
```

### Current Progress

- [x] Recursive directory traversal using `std::filesystem`.
- [x] Collecting files and directories.
- [x] Generating relative paths.
- [x] Preserving empty directory entries during traversal.
- [x] Detecting and rejecting symbolic links.
- [x] Defining archive magic bytes and format version.
- [x] Defining archive entry types.
- [x] Implementing raw binary read/write helpers.
- [x] Implementing 32-bit and 64-bit integer serialization.
- [x] Implementing length-prefixed string serialization.

### Planned Work

- [ ] Implement archive header serialization and validation.
- [ ] Implement directory and file entry serialization.
- [ ] Integrate the existing Huffman compressor with the directory archive format.
- [ ] Compress multiple files into a single archive.
- [ ] Implement recursive directory reconstruction during extraction.
- [ ] Add validation against malformed archives and unsafe extraction paths.
- [ ] Handle empty files, empty directories, and existing destination files.
- [ ] Add folder compression and extraction to the command-line interface.
- [ ] Integrate folder operations into Windows File Explorer.

### Proposed Archive Format

The folder archive will use a custom binary container.

The proposed structure is:

```text
Folder Archive
│
├── Archive Header
│   ├── Magic Bytes
│   ├── Format Version
│   ├── Entry Count
│   └── Root Directory Name
│
├── Directory Entry
│   ├── Entry Type
│   └── Relative Path
│
├── File Entry
│   ├── Entry Type
│   ├── Relative Path
│   ├── Compressed Payload Size
│   └── Huffman Compressed Data
│
└── Additional Entries...
```

Each file will be compressed independently using the existing Huffman implementation.

This approach allows individual files to be decoded independently and keeps the existing compression algorithm reusable.

The folder archive will also use its own magic header and version identifier to distinguish it from the existing single-file archive format.

## Project Structure

The intended project organization is:

```text
HuffmanCompressor/
│
├── src/
│   ├── compressor.cpp
│   └── compressor.hpp
│
├── assets/
│   └── HuffmanArchive.ico
│
├── installer/
│   └── HuffmanCompressor.iss
│
└── Output/
    └── HuffmanCompressorSetup.exe
```

## Future Improvements

- Recursive folder compression and extraction.
- Improved compression statistics and reporting.
- Progress tracking for large files and directories.
- Compression ratio reporting.
- Additional archive integrity checks.
- A graphical user interface.
- Further performance optimizations.

## License

Add your chosen license here before publicly distributing the project.
