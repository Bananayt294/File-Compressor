#include "compressor.hpp"

code* front = nullptr;
code* rear = nullptr;

namespace ArchiveIO {

    bool writeBytes(int fd, const void* data, size_t size)
    {
        const auto* bytes =
            static_cast<const unsigned char*>(data);

        while (size > 0) {

            unsigned int chunk = static_cast<unsigned int>(
                std::min(size, static_cast<size_t>(INT_MAX))
            );

            int written = _write(fd, bytes, chunk);

            if (written <= 0)
                return false;

            bytes += written;
            size -= written;
        }

        return true;
    }

    bool readBytes(int fd, void* data, size_t size) {
        auto* bytes = static_cast<unsigned char*>(data);

        while (size > 0) {

            unsigned int chunk = static_cast<unsigned int>(
                std::min(size, static_cast<size_t>(INT_MAX))
            );

            int bytesRead = _read(fd, bytes, chunk);

            if (bytesRead <= 0)
                return false;

            bytes += bytesRead;
            size -= bytesRead;
        }

        return true;
    }
    bool writeUInt32(int fd, uint32_t value) {
        std::array<unsigned char, 4> buffer{};

        for (size_t i = 0; i < 4; i++) {
            buffer[i] = static_cast<unsigned char>(
                (value >> (i * 8)) & 0xFF
            );
        }

        return writeBytes(fd, buffer.data(), buffer.size());
    }

    bool writeUInt64(int fd, uint64_t value) {
        std::array<unsigned char, 8> buffer{};

        for (size_t i = 0; i < 8; i++) {
            buffer[i] = static_cast<unsigned char>(
                (value >> (i * 8)) & 0xFF
            );
        }

        return writeBytes(fd, buffer.data(), buffer.size());
    }

    bool readUInt32(int fd, uint32_t& value) {
        std::array<unsigned char, 4> buffer{};

        if (!readBytes(fd, buffer.data(), buffer.size()))
            return false;

        value = 0;

        for (size_t i = 0; i < 4; i++) {
            value |= static_cast<uint32_t>(buffer[i]) << (i * 8);
        }

        return true;
    }

    bool readUInt64(int fd, uint64_t& value) {
        std::array<unsigned char, 8> buffer{};

        if (!readBytes(fd, buffer.data(), buffer.size()))
            return false;

        value = 0;

        for (size_t i = 0; i < 8; i++) {
            value |= static_cast<uint64_t>(buffer[i]) << (i * 8);
        }

        return true;
    }

}

void destroyTree(Tree* root)
{
    if (root == nullptr)
        return;

    // Destroy left subtree
    destroyTree(root->f);

    // Destroy right subtree
    destroyTree(root->r);

    // Destroy current node
    delete root;
}

void destroyHuffmanTree(Node* root) {
    if (root == nullptr) {
        return;
    }
    destroyHuffmanTree(root->l);
    destroyHuffmanTree(root->r);

    delete root;
}

void destroyCodeList() {
    code* current = front;
    while (current != nullptr) {
        code* next = current->p;
        delete current;
        current = next;
    }
    front = nullptr;
    rear = nullptr;
}

bool isroot(Tree* node) {
    return node != nullptr && node -> f == nullptr && node->r == nullptr;
};


int convertBinaryToDecimal(int arr[], int len) {
    int decimal = 0;
    for (int i = 0; i < len; i++) {
        decimal = decimal * 2 + arr[i];
    }
    return decimal;
}

int convertDecimalToBinary(int arr[], int decimal, int len) {
    for (int i = len - 1; i >= 0; i--) {
        arr[i] = decimal % 2;
        decimal /= 2;    
    }
    return decimal;
}

bool isLeaf(Node* root)
{
    return root != nullptr &&
           root->l == nullptr &&
           root->r == nullptr;
}

void insertIntoMin_Heap(Min_Heap* heap, Node* node) {
    int i = heap -> size;
    heap -> array[i] = node;
    heap -> size++;

    while(i != 0 && heap -> array[(i - 1)/ 2] -> freq > heap -> array[i] -> freq) {
        swap(heap -> array[i], heap -> array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

Node* extractMinFromMin_Heap(Min_Heap* heap) {
    if (heap -> size <= 0) {
        return nullptr;
    }
    Node* minNode = heap -> array[0];
    heap ->array[0] = heap -> array[heap -> size - 1];
    heap -> size--;
    if (heap -> size > 0) {
        Heapify(heap,0);
    };
    return minNode;
}

void swap(Node*& a, Node*& x) {
    Node* temp = a;
    a = x;
    x = temp;
}

void Heapify(Min_Heap* x, int i) {
    int smallest = i;
    
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < x -> size && x -> array[left]->freq < x -> array[smallest] -> freq) {
        smallest = left;
    }
    if (right < x -> size && x -> array[right] -> freq < x -> array[smallest] -> freq) {
        smallest = right;
    }
    if (smallest != i) {
        swap(x -> array[i], x -> array[smallest]);
        Heapify(x, smallest);
    }

}


void openFiles(const std::string& inputPath, const std::string& outputPath,int& fd1, int& fd2)
{
    fd1 = _open(inputPath.c_str(), _O_RDONLY | _O_BINARY);

    if (fd1 == -1) {
        perror("Failed to open input file");
        exit(1);
    }

    fd2 = _open(outputPath.c_str(),_O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY,_S_IREAD | _S_IWRITE);

    if (fd2 == -1) {
        perror("Failed to open output file");
        _close(fd1);
        exit(1);
    }
}

// Function to create min heap
Min_Heap* createAndBuildMin_Heap(char arr[], int freq[],
                                 int unique_size)
{
    int i;

    // Initializing heap
    Min_Heap* Heap = new Min_Heap(unique_size);

    // Initializing the array of pointers in minheap.
    // Pointers pointing to new nodes of character
    // and their frequency
    for (i = 0; i < unique_size; ++i) {
        Heap->array[i] = new Node(arr[i], freq[i]);
    }

    int n = Heap->size - 1;
    for (i = (n - 1) / 2; i >= 0; --i) {
        // Standard function for Heap creation
        Heapify(Heap, i);
    }

    return Heap;
}

bool isSizeOne(Min_Heap* x) {
    if (x -> size == 1) {
        return true;
    }
    return false;
}

Node* buildHuffmanTree(char arr[], int freq[], int unique_size)
{
    Min_Heap* heap = createAndBuildMin_Heap(arr, freq, unique_size);

    Node *l, *r, *top;

    while (!isSizeOne(heap))
    {
        l = extractMinFromMin_Heap(heap);
        r = extractMinFromMin_Heap(heap);

        top = new Node('$', l->freq + r->freq);

        top->l = l;
        top->r = r;

        insertIntoMin_Heap(heap, top);
    }

    Node* root = extractMinFromMin_Heap(heap);
    delete heap;
    return root;
}

void printCodesIntoFile(int fd2, struct Node* root,int t[], int top = 0){
    int i;
    if (root->l) {
        t[top] = 0;
        printCodesIntoFile(fd2, root->l, t, top + 1);
    }

    if (root->r) {
        t[top] = 1;
        printCodesIntoFile(fd2, root->r, t, top + 1);
    }

    if (isLeaf(root)) {
        auto data = (code*)malloc(sizeof(code));
        auto tree = (Tree*)malloc(sizeof(Tree));
        data->p = NULL;
        data->k = root->character;
        tree->g = root->character;
        write(fd2, &tree->g, sizeof(char));
        for (i = 0; i < top; i++) {
            data->code_arr[i] = t[i];
        }
        tree->len = top;
        write(fd2, &tree->len, sizeof(int));
        tree->dec = convertBinaryToDecimal(data->code_arr, top);
        write(fd2, &tree->dec, sizeof(int));
        data->l = top;
        data->p = NULL; 
        std::cout << "Writing character: "
        << root->character
        << " | Code length: "
        << top << '\n';
        if (front == nullptr) {
            front = rear = data;
        }
        else {
            rear->p = data;
            rear = data;
        }
    }
}


// Function to compress file
void compressFile(int fd1, int fd2, unsigned char a)
{
    char n;
    int h = 0, i;

    // Codes are written into file in bit by bit format
    while (read(fd1, &n, sizeof(char)) != 0) {
        rear = front;
        while (rear->k != n && rear->p != NULL) {
            rear = rear->p;
        }
        if (rear->k == n) {
            for (i = 0; i < rear->l; i++) {
                if (h < 7) {
                    if (rear->code_arr[i] == 1) {
                        a++;
                        a = a << 1;
                        h++;
                    }
                    else if (rear->code_arr[i] == 0) {
                        a = a << 1;
                        h++;
                    }
                }
                else if (h == 7) {
                    if (rear->code_arr[i] == 1) {
                        a++;
                        h = 0;
                    }
                    else {
                        h = 0;
                    }
                    write(fd2, &a, sizeof(char));
                    a = 0;
                }
            }
        }
    }
    for (i = 0; i < 7 - h; i++) {
        a = a << 1;
    }
    write(fd2, &a, sizeof(char));
}

Tree* ReBuildHuffmanTree(int fd1, int size)
{
    if (size <= 0 || size > 256)
        return nullptr;

    // Create root
    Tree* root = new Tree{};

    for (int k = 0; k < size; k++)
    {
        // Read metadata
        Tree record{};

        if (!ExtractCodesFromFile(fd1, record))
        {
            destroyTree(root);
            return nullptr;
        }

        // Validate metadata
        if (record.len <= 0 ||
            record.len > 31 ||
            record.dec < 0)
        {
            destroyTree(root);
            return nullptr;
        }

        // Convert decimal code into binary
        int bin[MAX]{};

        convertDecimalToBinary(
            bin,
            record.dec,
            record.len
        );

        // Start at root
        Tree* current = root;

        // Reconstruct path
        for (int j = 0; j < record.len; j++)
        {
            if (bin[j] == 0)
            {
                if (current->f == nullptr)
                    current->f = new Tree{};

                current = current->f;
            }
            else
            {
                if (current->r == nullptr)
                    current->r = new Tree{};

                current = current->r;
            }
        }

        // Store character at leaf
        current->g = record.g;
        current->len = record.len;
        current->dec = record.dec;
    }

    return root;
}

bool decompressFile(int fd1, int fd2, int f, Tree* root)
{
    int k = 0;
    unsigned char p;

    if (f <= 0)
        return false;

    // Check whether the Huffman tree exists
    if (root == nullptr)
    {
        std::cerr << "Huffman tree is missing" << std::endl;
        return false;
    }

    // Start traversal from the reconstructed root
    Tree* tree_temp = root;

    // Handle a file containing only one unique character
    if (isroot(root))
    {
        for (int i = 0; i < f; i++)
        {
            if (_write(fd2, &root->g, 1) != 1)
            {
                perror("Failed to write decompressed data");
                return false;
            }
        }

        return false;
    }

    while (k < f)
    {
        int bytesRead = _read(fd1, &p, 1);

        if (bytesRead != 1) {
            std::cerr << "Failed to read compressed data\n";
            return false;
        }

        // Process each bit, starting from the MSB
        for (int i = 7; i >= 0; i--) {
            int bit = (p >> i) & 1;

            if (bit == 0)
                tree_temp = tree_temp->f;
            else
                tree_temp = tree_temp->r;

            if (tree_temp == nullptr) {
                std::cerr << "Invalid Huffman bitstream\n";
                return false;
            }

            // Check whether we reached a leaf  
            if (isroot(tree_temp)) {
                if (_write(fd2, &tree_temp->g, 1) != 1) {
                    perror("Failed to write decompressed data");
                    return true;
                }

                k++;

                // Start decoding the next character
                tree_temp = root;

                if (k == f)
                    break;
            }
        }
    }
    return true;
}

bool ExtractCodesFromFile(int fd1, Tree& record)
{
    // Read the character
    if (_read(fd1, &record.g, sizeof(char)) != sizeof(char))
        return false;

    // Read the Huffman code length
    if (_read(fd1, &record.len, sizeof(int)) != sizeof(int))
        return false;

    // Read the decimal representation
    if (_read(fd1, &record.dec, sizeof(int)) != sizeof(int))
        return false;

    return true;
}

int compressFileFromPath(const std::string& inputPath) {
    int fd1 = _open(inputPath.c_str(), O_RDONLY | O_BINARY);
    if (fd1 == -1) {
        perror("Failed to open input File");
        return 1;
    }
    int frequencies[MAX]{};
    int original_size = 0;
    unsigned char ch;
    int bytesRead;
    while ((bytesRead = _read(fd1, &ch, 1)) == 1) {
        frequencies[ch]++;
        original_size++;
    }
    if (bytesRead == -1) {
        perror("Failed to read InputFile");
        _close(fd1);
        return 1;
    }
    char arr [MAX];
    int freq[MAX];

    int unique_size = 0;

        for (int i = 0; i < MAX; i++) {
            if (frequencies[i] > 0) {
                arr[unique_size] = static_cast<char>(i);
                freq[unique_size] = frequencies[i];

                unique_size++;
            }
        }
            std::cout << "Original size: " << original_size << '\n';
            std::cout << "Unique characters: " << unique_size << '\n';
        if (unique_size < 2) {
            std::cerr << "Need at least two unique characters.\n";
            _close(fd1);
            return 1;
        }
        Node* root = buildHuffmanTree(arr, freq, unique_size);
        if (root == nullptr) {
            std::cerr << "Failed to build Huffman tree." << std::endl;
            _close(fd1);
            return 1;
        }
        std::string outputPath = inputPath + ".huff";
        int fd2 = _open(outputPath.c_str(), _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
        if (fd2 == -1) {
            perror("failed to create Compressed File");
            destroyHuffmanTree(root);
            _close(fd1);
            return 1;
        } 
        if (_write(fd2, &original_size, sizeof(int)) != sizeof(int) || _write(fd2, &unique_size, sizeof(int)) != sizeof(int)) {
            perror("Failed to write File header");
            _close(fd1);
            _close(fd2);
            destroyHuffmanTree(root);
            return 1;
        }
        std::cout << "Header written!\n";
        std::cout << "Current output position: " << _tell(fd2) << '\n';
        int t[MAX]{};
        front = nullptr;
        rear = nullptr;
        printCodesIntoFile(fd2, root, t, 0);
        if (_lseek(fd1, 0, SEEK_SET) == -1) {
            perror("Failed to reset input File");
            _close(fd1);
            _close(fd2);
            destroyHuffmanTree(root);
            return 1;
        }
        compressFile(fd1, fd2, 0);
        _close(fd1);
        _close(fd2);
        destroyHuffmanTree(root);
        std::cout << "Compression completed" << std::endl;
        std::cout << "Output" << outputPath << std::endl;
        return 0;
    }

std::optional<std::filesystem::path> chooseExtractionPath(const std::filesystem::path& suggestedPath) {
    std::array<wchar_t, 32768> fileBuffer{};

    std::wstring filename = suggestedPath.filename().wstring();

    if (filename.size() >= fileBuffer.size()) {
        std::cerr << "Filename is too long." << std::endl;
        return std::nullopt;
    }

    std::copy(filename.begin(), filename.end(), fileBuffer.begin());

    std::wstring initialDirectory = suggestedPath.parent_path().wstring();

    std::wstring defaultExtension = suggestedPath.extension().wstring();

    if (!defaultExtension.empty() && defaultExtension.front() == L'.') {
        defaultExtension.erase(0, 1);
    }

    static constexpr wchar_t filter[] =
        L"All files (*.*)\0*.*\0\0";

    OPENFILENAMEW ofn{};

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileBuffer.data();
    ofn.nMaxFile = static_cast<DWORD>(fileBuffer.size());

    ofn.lpstrInitialDir = initialDirectory.empty()
        ? nullptr
        : initialDirectory.c_str();

    ofn.lpstrTitle = L"Choose extracted filename";

    ofn.lpstrDefExt = defaultExtension.empty()
        ? nullptr
        : defaultExtension.c_str();

    ofn.Flags =
        OFN_EXPLORER |
        OFN_PATHMUSTEXIST |
        OFN_NOCHANGEDIR |
        OFN_HIDEREADONLY;

    if (!GetSaveFileNameW(&ofn)) {

        DWORD error = CommDlgExtendedError();

        if (error != 0) {
            std::cerr << "Save dialog error: " << error << std::endl;
        }

        return std::nullopt;
    }

    return std::filesystem::path(fileBuffer.data());
}

int extractFileFromPath(const std::string& archivepath) {
    std::filesystem::path inputPath = archivepath;
    if (inputPath.extension() != ".huff") {
        std::cerr << "Invalid archive extension. Expected .huff" << std::endl;
        return 1;
    }
    int fd1 = _open(archivepath.c_str(), O_RDONLY | O_BINARY);
    if (fd1 == -1) {
        perror("failed to open compressed archive");
        return 1;
    }
    int original_size{};
    int unique_size{};

    if (_read(fd1, &original_size, sizeof(int)) != sizeof(int)) {
        std::cerr << "Failed to read orginal file size" << std::endl;
        _close(fd1);
        return 1;
    }
    if (_read(fd1, &unique_size, sizeof(int)) != sizeof(int)) {
        std::cerr << "Failed to read unique character count" << std::endl;
        _close(fd1);
        return 1;
    }

    std::cout << "Original size: " << original_size << '\n';
    std::cout << "Unique size: " << unique_size << '\n';
    if (original_size <= 0 || unique_size < 2 || unique_size > MAX) {
        std::cerr << "Invalid or unsupported archive" << std::endl;
        _close(fd1);
        return 1;
    }
    Tree* root = ReBuildHuffmanTree(fd1, unique_size);
    if (root == nullptr) {
        std::cerr << "Failed to reconstruct Huffman tree" << std::endl;
        _close(fd1);
        return 1;
    }

    std::filesystem::path outputPath = inputPath;
    outputPath.replace_extension();

    int fd2 = -1;

    while (fd2 == -1) {

        std::error_code error;

        bool exists = std::filesystem::exists(outputPath, error);

        if (error) {
            std::cerr << "Failed to check output path: "
                    << error.message() << std::endl;

            destroyTree(root);
            _close(fd1);
            return 1;
        }

        if (exists) {

            std::wcout << L"File already exists: "
                    << outputPath.wstring() << std::endl;

            auto selectedPath = chooseExtractionPath(outputPath);

            if (!selectedPath) {
                std::cout << "Extraction cancelled." << std::endl;

                destroyTree(root);
                _close(fd1);
                return 1;
            }

            outputPath = *selectedPath;
            continue;
        }
        fd2 = _wopen(
            outputPath.c_str(),
            _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY,
            _S_IREAD | _S_IWRITE
        );
        if (fd2 == -1 && errno != EEXIST) {
            perror("Failed to create output file");

            destroyTree(root);
            _close(fd1);
            return 1;
        }
    }
    bool success = decompressFile(fd1, fd2, original_size, root);
    _close(fd1); 
    _close(fd2);

    destroyTree(root);
    if (!success) {
        std::cerr << "Extraction failed" << std::endl;
        std::error_code error;
        std::filesystem::remove(outputPath, error);
        return 1;
    }
    std::cout << "Extraction Completed" << std::endl;
    std::cout << "Ouput: " << outputPath.string() << std::endl;
    return 0;
}

std::optional<std::vector<ArchiveEntry>> collectDirectoryEntries(const std::filesystem::path& folderPath, std::string& errorMessage)
{
    namespace fs = std::filesystem;

    errorMessage.clear();

    std::error_code ec;

    fs::path root = fs::absolute(folderPath, ec).lexically_normal();

    if (ec) {
        errorMessage = "Failed to resolve directory path: " + ec.message();
        return std::nullopt;
    }

    fs::file_status rootStatus = fs::symlink_status(root, ec);

    if (ec || !fs::is_directory(rootStatus) || fs::is_symlink(rootStatus)) {
        errorMessage = "Invalid directory path.";
        return std::nullopt;
    }

    std::vector<ArchiveEntry> entries;

    fs::recursive_directory_iterator it(root, ec);
    fs::recursive_directory_iterator end;

    if (ec) {
        errorMessage = "Failed to open directory: " + ec.message();
        return std::nullopt;
    }

    while (it != end) {

        fs::path currentPath = it->path();

        fs::file_status status = it->symlink_status(ec);

        if (ec) {
            errorMessage = "Failed to inspect: " +
                currentPath.string() + ": " + ec.message();

            return std::nullopt;
        }

        if (fs::is_symlink(status)) {
            errorMessage = "Symbolic links are not supported yet: " +
                currentPath.string();

            return std::nullopt;
        }

        bool isDirectory = fs::is_directory(status);
        bool isFile = fs::is_regular_file(status);

        if (!isDirectory && !isFile) {
            errorMessage = "Unsupported filesystem entry: " +
                currentPath.string();

            return std::nullopt;
        }

        fs::path relativePath = currentPath.lexically_relative(root);

        if (relativePath.empty()) {
            errorMessage = "Failed to generate relative path.";
            return std::nullopt;
        }

        entries.push_back({
            relativePath,
            isDirectory
        });

        it.increment(ec);

        if (ec) {
            errorMessage = "Directory traversal failed: " + ec.message();
            return std::nullopt;
        }
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [](const ArchiveEntry& a, const ArchiveEntry& b) {
            return a.relativePath < b.relativePath;
        }
    );

    return entries;
}



int main(int argc, char* argv[])
{   
    if(argc == 3) {
        std::string operation = argv[1];
        std::string filePath = argv[2];
        

    if (operation == "compress") {

        std::string errorMessage;

        auto entries = collectDirectoryEntries(filePath, errorMessage);

        if (!entries) {
            std::cerr << errorMessage << std::endl;
            return 1;
        }

        for (const auto& entry : *entries) {

            if (entry.isDirectory) {
                std::cout << "[DIR]  ";
            }
            else {
                std::cout << "[FILE] ";
            }

            std::cout << entry.relativePath.string() << '\n';
        }

        return 0;
    }
        else if (operation == "extract") {
            return extractFileFromPath(filePath);
        }
        else {
            std::cerr << "Invalid operation!" << std::endl;
            return 1;
        }
        return 0;
}


    // int choice;
    // using namespace std;
    // cout << "========== HUFFMAN COMPRESSOR ==========\n";
    // cout << "1. Compress a file\n";
    // cout << "2. Decompress a file\n";
    // cout << "Enter your choice: ";

    // std::cin >> choice;

    // // =====================================
    // // COMPRESSION
    // // =====================================

    // if (choice == 1)
    // {
    //     int fd1{};
    //     int fd2{};

    //     openFiles(fd1, fd2);

    //     int frequencies[256]{};
    //     int original_size = 0;

    //     unsigned char ch;
    //     int bytesRead;

    //     while ((bytesRead = _read(fd1, &ch, 1)) == 1)
    //     {
    //         frequencies[ch]++;
    //         original_size++;
    //     }

    //     if (bytesRead == -1)
    //     {
    //         perror("Failed to read input file");
    //         _close(fd1);
    //         _close(fd2);
    //         return 1;
    //     }

    //     char arr[256];
    //     int freq[256];

    //     int unique_size = 0;

    //     for (int i = 0; i < 256; i++)
    //     {
    //         if (frequencies[i] > 0)
    //         {
    //             arr[unique_size] = static_cast<char>(i);
    //             freq[unique_size] = frequencies[i];

    //             unique_size++;
    //         }
    //     }

    //     if (unique_size < 2)
    //     {
    //         std::cerr << "Need at least two unique characters.\n";
    //         _close(fd1);
    //         _close(fd2);
    //         return 1;
    //     }
    //     Node* root = buildHuffmanTree(arr, freq, unique_size);

    //     if (root == nullptr)
    //     {
    //         std::cerr << "Failed to build Huffman tree.\n";
    //         _close(fd1);
    //         _close(fd2);
    //         return 1;
    //     }

    //     _write(fd2, &original_size, sizeof(int));
    //     _write(fd2, &unique_size, sizeof(int));

    //     int t[MAX]{};

    //     front = nullptr;
    //     rear = nullptr;

    //     printCodesIntoFile(fd2, root, t, 0);

    //     _lseek(fd1, 0, SEEK_SET);

    //     compressFile(fd1, fd2, 0);

    //     cout << "\nCompression completed!\n";

    //     _close(fd1);
    //     _close(fd2);
    // }

    // else if (choice == 2) {

    //     int fd1 = _open(
    //         "sample-compressed.txt",
    //         _O_RDONLY | _O_BINARY
    //     );

    //     if (fd1 == -1)
    //     {
    //         perror("Failed to open compressed file");
    //         return 1;
    //     }

    //     int fd2 = _open(
    //         "sample-decompressed.txt",
    //         _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY,
    //         _S_IREAD | _S_IWRITE
    //     );

    //     if (fd2 == -1) {
    //         perror("Failed to open output file");
    //         _close(fd1);
    //         return 1;
    //     }


    //     int original_size;

    //     if (_read(fd1, &original_size, sizeof(int)) != sizeof(int)) {
    //         cerr << "Failed to read original file size\n";
    //         _close(fd1);
    //         _close(fd2);
    //         return 1;
    //     }

    //     int unique_size;

    //     if (_read(fd1, &unique_size, sizeof(int)) != sizeof(int)) {
    //         cerr << "Failed to read unique character count\n";
    //         _close(fd1);
    //         _close(fd2);
    //         return 1;
    //     }

    //     cout << "Original size: " << original_size << '\n';
    //     cout << "Unique characters: " << unique_size << '\n';

    //     Tree* root = ReBuildHuffmanTree(fd1, unique_size);

    //     if (root == nullptr) {
    //         cerr << "Tree reconstruction failed!\n";
    //         return 1;
    //     }

    //     cout << "Root: " << root << '\n';
    //     cout << "Left child: " << root->f << '\n';
    //     cout << "Right child: " << root->r << '\n';    

    //     if (original_size > 0 && root == nullptr) {
    //         cerr << "Failed to rebuild Huffman tree\n";
    //         _close(fd1);
    //         _close(fd2);
    //         return 1;
    //     }

    //     decompressFile(fd1, fd2, original_size, root);

    //     cout << "\nDecompression completed!\n";
    //     destroyTree(root);

    //     _close(fd1);
    //     _close(fd2);
    // }

    // else {
    //     cout << "Invalid choice!\n";
    //     return 1;
    // }

    // return 0;
}