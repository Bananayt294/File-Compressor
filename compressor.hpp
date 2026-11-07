#include <iostream>
#include <utility>
#include <vector>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
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

void swap(Node*& a, Node*& b);
void Heapify(Min_Heap* heap, int i);
bool ExtractCodesFromFile(int fd1, Tree& record);

extern code* front;
extern code* rear;