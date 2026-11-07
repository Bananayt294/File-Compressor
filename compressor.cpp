#include "compressor.hpp"

code* front = nullptr;
code* rear = nullptr;

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


void openFiles(int& fd1, int& fd2)
{
    fd1 = _open("sample.txt", _O_RDONLY | _O_BINARY);

    if (fd1 == -1) {
        perror("Failed to open input file");
        exit(1);
    }

    fd2 = _open("sample-compressed.txt",_O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY,_S_IREAD | _S_IWRITE);

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

    return extractMinFromMin_Heap(heap);
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

void decompressFile(int fd1, int fd2, int f, Tree* root)
{
    int k = 0;
    unsigned char p;

    if (f <= 0)
        return;

    // Check whether the Huffman tree exists
    if (root == nullptr)
    {
        std::cerr << "Huffman tree is missing" << std::endl;
        return;
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
                return;
            }
        }

        return;
    }

    while (k < f)
    {
        int bytesRead = _read(fd1, &p, 1);

        if (bytesRead != 1)
        {
            std::cerr << "Failed to read compressed data\n";
            return;
        }

        // Process each bit, starting from the MSB
        for (int i = 7; i >= 0; i--)
        {
            int bit = (p >> i) & 1;

            if (bit == 0)
                tree_temp = tree_temp->f;
            else
                tree_temp = tree_temp->r;

            if (tree_temp == nullptr)
            {
                std::cerr << "Invalid Huffman bitstream\n";
                return;
            }

            // Check whether we reached a leaf
            if (isroot(tree_temp))
            {
                if (_write(fd2, &tree_temp->g, 1) != 1)
                {
                    perror("Failed to write decompressed data");
                    return;
                }

                k++;

                // Start decoding the next character
                tree_temp = root;

                if (k == f)
                    break;
            }
        }
    }
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


int main()
{
    int choice;
    using namespace std;
    cout << "========== HUFFMAN COMPRESSOR ==========\n";
    cout << "1. Compress a file\n";
    cout << "2. Decompress a file\n";
    cout << "Enter your choice: ";

    std::cin >> choice;

    // =====================================
    // COMPRESSION
    // =====================================

    if (choice == 1)
    {
        int fd1{};
        int fd2{};

        openFiles(fd1, fd2);

        // STEP 1: Count frequencies

        int frequencies[256]{};
        int original_size = 0;

        unsigned char ch;
        int bytesRead;

        while ((bytesRead = _read(fd1, &ch, 1)) == 1)
        {
            frequencies[ch]++;
            original_size++;
        }

        if (bytesRead == -1)
        {
            perror("Failed to read input file");
            _close(fd1);
            _close(fd2);
            return 1;
        }

        // STEP 2: Create character and frequency arrays

        char arr[256];
        int freq[256];

        int unique_size = 0;

        for (int i = 0; i < 256; i++)
        {
            if (frequencies[i] > 0)
            {
                arr[unique_size] = static_cast<char>(i);
                freq[unique_size] = frequencies[i];

                unique_size++;
            }
        }

        if (unique_size < 2)
        {
            std::cerr << "Need at least two unique characters.\n";
            _close(fd1);
            _close(fd2);
            return 1;
        }

        // STEP 3: Build Huffman tree

        Node* root = buildHuffmanTree(arr, freq, unique_size);

        if (root == nullptr)
        {
            std::cerr << "Failed to build Huffman tree.\n";
            _close(fd1);
            _close(fd2);
            return 1;
        }

        // STEP 4: Write file metadata

        _write(fd2, &original_size, sizeof(int));
        _write(fd2, &unique_size, sizeof(int));

        // STEP 5: Generate Huffman codes

        int t[MAX]{};

        front = nullptr;
        rear = nullptr;

        printCodesIntoFile(fd2, root, t, 0);

        // STEP 6: Reset input file position

        _lseek(fd1, 0, SEEK_SET);

        // STEP 7: Compress file

        compressFile(fd1, fd2, 0);

        cout << "\nCompression completed!\n";

        _close(fd1);
        _close(fd2);
    }
    
    else if (choice == 2)
    {
        // STEP 1: Open compressed file

        int fd1 = _open(
            "sample-compressed.txt",
            _O_RDONLY | _O_BINARY
        );

        if (fd1 == -1)
        {
            perror("Failed to open compressed file");
            return 1;
        }

        // STEP 2: Open output file

        int fd2 = _open(
            "sample-decompressed.txt",
            _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY,
            _S_IREAD | _S_IWRITE
        );

        if (fd2 == -1) {
            perror("Failed to open output file");
            _close(fd1);
            return 1;
        }

        // STEP 3: Read original file size

        int original_size;

        if (_read(fd1, &original_size, sizeof(int)) != sizeof(int)) {
            cerr << "Failed to read original file size\n";
            _close(fd1);
            _close(fd2);
            return 1;
        }

        // STEP 4: Read unique character count

        int unique_size;

        if (_read(fd1, &unique_size, sizeof(int)) != sizeof(int)) {
            cerr << "Failed to read unique character count\n";
            _close(fd1);
            _close(fd2);
            return 1;
        }

        cout << "Original size: " << original_size << '\n';
        cout << "Unique characters: " << unique_size << '\n';

        // STEP 5: Rebuild Huffman tree

        Tree* root = ReBuildHuffmanTree(fd1, unique_size);

        if (root == nullptr) {
            cerr << "Tree reconstruction failed!\n";
            return 1;
        }

        cout << "Root: " << root << '\n';
        cout << "Left child: " << root->f << '\n';
        cout << "Right child: " << root->r << '\n';    

        if (original_size > 0 && root == nullptr) {
            cerr << "Failed to rebuild Huffman tree\n";
            _close(fd1);
            _close(fd2);
            return 1;
        }

        // STEP 6: Decompress

        decompressFile(fd1, fd2, original_size, root);

        cout << "\nDecompression completed!\n";

        // STEP 7: Destroy reconstructed tree

        destroyTree(root);

        _close(fd1);
        _close(fd2);
    }

    else {
        cout << "Invalid choice!\n";
        return 1;
    }

    return 0;
}