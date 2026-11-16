# Huffman Compressor

A Windows file compression and decompression utility written in C++ using Huffman coding. It supports individual files and integrates with Windows File Explorer through right-click context menus.

Folder compression and extraction are currently in development.

> **Development notice:** Folder support is not ready yet. Running `compressor.exe compress "<directory>"` currently lists the directory's contents without creating an archive. This is expected behavior. Compression and extraction of supported individual files are working.

**Compressing a file:** Right-click a supported file in File Explorer and select **Compress with Huffman Compressor**. The resulting `.huff` archive is saved in the same directory as the original file.

**Extracting an archive:** Right-click a `.huff` file and select **Extract with Huffman Compressor**, or double-click the archive. The extracted file is saved beside the archive. If a file with the same name already exists, you can choose a different location.

## Features

### Huffman Compression

- Uses Huffman coding for lossless compression.
- Builds the Huffman tree using a custom Min Heap.
- Generates variable-length binary codes based on character frequencies.
- Packs encoded bits into bytes before writing them to the archive.
- Stores the metadata needed to reconstruct the Huffman tree during extraction.
- Uses Windows file APIs for binary file operations.

### Decompression

- Rebuilds the Huffman tree using the metadata stored in the archive.
- Decodes the compressed bitstream.
- Restores the original file contents.
- Checks archive metadata before starting decompression.

### Archive Management

- Uses the `.huff` file extension for compressed archives.
- Creates archives from supported regular files.
- Extracts files from existing archives.
- Checks for existing files before extraction and lets the user choose another output path.
- Opens the native Windows Save As dialog when the original filename already exists.
- Uses exclusive file creation (`_O_EXCL`) to prevent existing files from being overwritten.
- Currently requires at least two distinct byte values in the input. Empty files and files containing only one repeated byte cannot be compressed.
- Uses 32-bit signed counters, so files close to or larger than 2 GiB are not supported.
- Overwrites an existing `<input>.huff` archive when compressing the same input again.
- Does not guarantee that every file will become smaller. For some files, the Huffman metadata can make the compressed archive larger than the original.

### Windows Integration

The installer adds the following options to Windows File Explorer:

- **Compress with Huffman Compressor:** Compress a supported regular file.
- **Extract with Huffman Compressor:** Extract a `.huff` archive.
- Double-clicking a `.huff` file also starts extraction.
- Custom icon for `.huff` archives.
- Start Menu shortcuts.
- Optional desktop shortcut.
- Automatic uninstaller.

### Installer

The application is packaged using Inno Setup.

- Installs under `%LOCALAPPDATA%\Programs\Huffman Compressor`.
- Registers `.huff` file associations and Explorer context-menu commands.
- Installs the custom archive icon.
- Creates an uninstaller and registers the application with Windows.
- Does not require administrator privileges.

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

### Compression

When compressing a file, the program:

1. Opens the input file in binary mode.
2. Reads the file and calculates the frequency of each byte.
3. Builds a Min Heap using those frequencies.
4. Constructs the Huffman tree by repeatedly extracting the two nodes with the lowest frequencies.
5. Generates a Huffman code for each unique byte.
6. Writes the original file size, number of unique byte values, and Huffman metadata to the archive.
7. Encodes the input using the generated codes.
8. Packs the resulting bits into bytes and writes them to the archive.

### Decompression

Extraction reverses the process:

1. Opens the `.huff` archive.
2. Reads the original file size and number of unique byte values.
3. Reconstructs the Huffman tree from the stored metadata.
4. Reads the compressed bitstream.
5. Traverses the tree to decode the original bytes.
6. Writes the recovered contents to the output file.

### Preventing File Overwrites

Before extracting an archive, the program checks whether the intended output filename already exists.

If it does, the native Windows Save As dialog is opened using `GetSaveFileNameW()`. The user can choose another filename or cancel the operation.

The program also uses exclusive file creation flags (`_O_EXCL`). This prevents an existing file from being overwritten, even if it is created after the initial filename check.

## Building From Source

### Requirements

- Windows
- A C++20-compatible compiler
- MSYS2 UCRT64 / MinGW-w64 G++
- Windows Common Dialog library (`comdlg32`)

### Compilation

Run the following command from the source directory:

```bash
g++ -std=c++20 -O2 compressor.cpp -o compressor.exe -lcomdlg32
```

### Command-Line Usage

**Compress a file:**

```powershell
.\compressor.exe compress "C:\path\to\file.txt"
```

The resulting `.huff` archive is created beside the input file.

The current implementation requires at least two distinct byte values. Empty files and files containing only one repeated byte are not supported.

Passing a directory to the `compress` command only lists its entries. It does not create a folder archive yet.

**Extract an archive:**

```powershell
.\compressor.exe extract "C:\path\to\file.txt.huff"
```

The program extracts the contents to `file.txt` beside the archive. If that file already exists, it opens the Save As dialog so another output path can be selected.

The command-line interface currently expects an operation and a path. Running the executable without arguments does not open an interactive menu.

## Installing and Using a Release

1. Download `HuffmanCompressorSetup.exe` from the [latest GitHub release](https://github.com/Bananayt294/File-Compressor/releases/latest).
2. Run the installer. The application is installed for the current Windows user under `%LOCALAPPDATA%\Programs\Huffman Compressor`. Administrator privileges are not required.
3. To compress a file, right-click it in File Explorer and select **Compress with Huffman Compressor**. The `.huff` archive will be created in the same directory. An existing archive with the same name will be overwritten.
4. To extract an archive, right-click the `.huff` file and select **Extract with Huffman Compressor**, or double-click it. The original filename is restored in the archive's directory. If that filename already exists, the Save As dialog lets you choose another path.

A few things to keep in mind:

- Empty files cannot currently be compressed.
- Files containing only one repeated byte cannot currently be compressed.
- Files close to or above 2 GiB are not supported.
- Folder compression and extraction are still in development.
- The Explorer compression command is intended for individual files. Passing a directory to the command-line `compress` operation only lists its contents.

You can also run the installed program directly from PowerShell:

```powershell
& "$env:LOCALAPPDATA\Programs\Huffman Compressor\compressor.exe" compress "C:\path\to\file.txt"

& "$env:LOCALAPPDATA\Programs\Huffman Compressor\compressor.exe" extract "C:\path\to\file.txt.huff"
```

## Building the Installer

The installer is built with Inno Setup.

1. Open `HuffmannCompressor.iss` in Inno Setup.
2. Make sure the latest compiled `compressor.exe` is in the source directory expected by the script.
3. Press `F9` to compile the installer.
4. Find the generated setup executable in the output directory.

The output is organized as follows:

```text
Output/
└── HuffmanCompressorSetup.exe
```

The Inno Setup script takes care of installing the application, registering the file association and Explorer commands, creating shortcuts, and configuring the uninstaller.

## Folder Compression and Extraction

Folder support is the next major part of the project. The goal is to compress an entire directory into a single archive while keeping its directory structure intact when extracted.

For example, given this directory:

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

The expected archive is:

```text
MyFolder.huff
```

After extraction, the original structure should be restored:

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

The directory traversal and basic archive I/O components are already in place.

- [x] Recursive directory traversal using `std::filesystem`.
- [x] Collection of files and directories.
- [x] Generation of relative paths.
- [x] Preservation of empty directory entries during traversal.
- [x] Detection and rejection of symbolic links.
- [x] Archive magic bytes and format version definitions.
- [x] Archive entry type definitions.
- [x] Raw binary read/write helpers.
- [x] 32-bit and 64-bit integer serialization.
- [x] Length-prefixed string serialization.

### Remaining Work

- [ ] Implement archive header serialization and validation.
- [ ] Serialize directory and file entries.
- [ ] Connect the existing Huffman compressor to the folder archive format.
- [ ] Compress multiple files into one archive.
- [ ] Recreate directory structures during extraction.
- [ ] Validate archives against malformed data and unsafe extraction paths.
- [ ] Handle empty files, empty directories, and existing destination files.
- [ ] Add folder compression and extraction to the command-line interface.
- [ ] Add folder operations to the Windows File Explorer context menu.

### Proposed Archive Format

The folder archive will use a custom binary format:

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

Each file will be compressed separately using the existing Huffman implementation. This keeps the compression code reusable and allows files to be decoded independently.

The folder archive will have its own magic header and version identifier, separate from the existing single-file archive format.

## Project Structure

The project is intended to follow this structure:

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
│   └── HuffmannCompressor.iss
│
└── Output/
    └── HuffmanCompressorSetup.exe
```

## Future Improvements

There are still a few things planned for the project:

- Recursive folder compression and extraction.
- Compression statistics and reporting.
- Progress tracking for large files and directories.
- Compression ratio reporting.
- Additional archive integrity checks.
- A graphical user interface.
- Further performance optimizations.

## License

MIT License