#include <iostream>
#include <utility>
#include <vector>
#include <memory>

using namespace std;

struct Node {
    char character;
    int freq;
    Node *l,*r;
    Node(char c, int f): character(c), freq(f), l(nullptr), r(nullptr){};
};

struct Min_Heap {
    int size;
    vector<Node*> array;

    Min_Heap(int s) : size(s), array(s){};
};

bool isSizeOne(Min_Heap x){
    if (x.size == 1) {
        return true;
    };   
    return false;
};

Min_Heap* createAndBuildMinHeap(char arr[], int freq[], int unique_size) {
    int i;

    Min_Heap* Min_Heap = new Min_Heap(unique_size);

    for (i = 0; i < unique_size; ++i) {
        Min_Heap -> array[i] = new Node(arr[i], freq[i]);
    }
    int n = Min_Heap -> size -1 ;
    for (i = (n-1)/2; i >= 0; --i) {
        Heapify(Min_Heap, i);
    };
    return Min_Heap;
}

struct Node* BuildHuffmannTree(char arr[], int freq[], int unique_size) {
    struct Node *l,*r,*top;
    while (!isSizeOne(Min_Heap)){
        l = extractMinfromMin_Heap(Min_Heap);
        r = extractMinfromMin_Heap(Min_Heap);
        top = newNode('$', l->freq, r->freq);
        top -> l  = l;
        top -> r = r;
        insertIntoMin_Heap(Min_Heap, top);
    }
    return extractMinFromMin_Heap(Min_Heap);
}

typedef struct code {
    char k;
    int l;
    int code_arr[16];
    struct code* p;
}code;

void printCodesIntoFile(int fd2, struct Node* root, int t[], int top = 0){
    int i;
    if (root -> l) {
        t[top] = 0;
        printCodesIntoFile(fd2, root -> l, t, top + 1);
    }
    if (root -> r) {
        t[top] = 1;
        printCodesIntoFile(fd2, root -> r, t, top + 1);
    }
}