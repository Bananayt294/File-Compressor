#define NOMINMAX
#include <iostream>
#include <utility>
#include <vector>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <filesystem>
#include <windows.h>
#include <commdlg.h>
#include <array>
#include <algorithm>
#include <optional>
#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <system_error>
constexpr int MAX = 256;
// Structure for tree nodes
struct Node {
    char character;
    int freq;
    Node *l, *r;

    Node(char c, int f)
        : character(c)
        , freq(f)
        , l(nullptr)
        , r(nullptr)
    {
    }
};

// Structure for min heap
struct Min_Heap {
    int size;
    std::vector<Node*> array;

    Min_Heap(int s)
        : size(s)
        , array(s)
    {
    }
};

typedef struct code {
    char k;
    int l;
    int code_arr[16];
    struct code* p;
} code;

// Function to print codes into file

typedef struct Tree {
    char g;
    int len;
    int dec;
    struct Tree* f;
    struct Tree* r;
} Tree;

struct ArchiveEntry {
    std::filesystem::path relativePath;
    bool isDirectory;
};

enum class ArchiveEntryType : uint8_t {
    Directory = 0,
    File = 1
}; 

constexpr std::array<char, 8> FOLDER_ARCHIVE_MAGIC = {
    'H', 'U', 'F', 'F', 'D', 'I', 'R', '\0'
};

constexpr uint32_t FOLDER_ARCHIVE_VERSION = 1;

constexpr uint32_t MAX_ARCHIVE_ENTRIES = 1'000'000;

constexpr uint32_t MAX_PATH_LENGTH = 32768;



void swap(Node*& a, Node*& b);
void Heapify(Min_Heap* heap, int i);
bool ExtractCodesFromFile(int fd1, Tree& record);
int compressFileFromPath(const std::string& inputPath);

extern code* front;
extern code* rear;