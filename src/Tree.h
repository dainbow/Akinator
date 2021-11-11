#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "Text.h"

typedef uint8_t* TreeElem;
#define TREE_TYPE "%s"

#define _ENABLE_PTR_ADRESSES_VIEW_ 0

#if (_ENABLE_PTR_ADRESSES_VIEW_ == 1)
    #define LEFT_BRANCH  "%lld"
    #define RIGHT_BRANCH "%lld"
    #define LEFT_BRANCH_VALUE  root->left  - pointerAnchor
    #define RIGHT_BRANCH_VALUE root->right - pointerAnchor
#else
    #define LEFT_BRANCH  "l%d"
    #define RIGHT_BRANCH "r%d"
    #define LEFT_BRANCH_VALUE  curNodeNumber
    #define RIGHT_BRANCH_VALUE curNodeNumber
#endif


struct Node {
    TreeElem data;

    Node* left;
    Node* right;
};

struct Tree {
    Node* root;
    Text qbase;
};

const int32_t MAX_RECURSION_DEPTH = 1000;
const char G_STANDART_NAME[] = "graph";

#define TreeCtor(tree)    \
    Tree tree;            \
    TreeCtor_(&tree)

#define _DEBUG_MODE_ 1

#define IsTreeOk(tree)                          \
    if (VerifyTree(tree)) {                     \
        MakeTreeGraph(tree);                    \
        assert(FAIL && "CHECK TREE DUMP");      \
    }

void TreeCtor_(Tree* tree);

void TreeDtor(Tree* tree);
void NodeDtor(Node* root);

Node* MakeNewNode(TreeElem data, Node* left = nullptr, Node* right = nullptr);

void PrintTreeNodes(Node* root, FILE* output = stdout);
bool VerifyTree(Node* root);
