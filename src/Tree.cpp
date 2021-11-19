#include "Tree.h"
#include "Graph.h"
#include "Akinator.h"

void TreeCtor_(Tree* tree) {
    assert(tree != nullptr);

    tree->qbase = {};
    tree->root = (Node*)calloc(1, sizeof(tree->root[0]));

    tree->unsavedQuestions = (int8_t*)calloc(MAX_BUFFER_SIZE, sizeof(tree->unsavedQuestions[0]));
    tree->bufLen           = 0;
}

void TreeDtor(Tree* tree) {
    assert(tree != nullptr);

    free(tree->unsavedQuestions);

    tree->qbase  = {};
    tree->bufLen = 0;

    NodeDtor(tree->root);
}

void NodeDtor(Node* root) {
    assert(root != nullptr);

    if (root->left  != nullptr) NodeDtor(root->left);
    if (root->right != nullptr) NodeDtor(root->right);

    root->data  = (TreeElem)13;
    root->left  = (Node*)13;
    root->right = (Node*)13;
    free(root);
}

Node* MakeNewNode(TreeElem data, Node* left, Node* right) {
    assert((left == nullptr) == (right == nullptr));

    Node* newNode = (Node*)calloc(1, sizeof(newNode[0]));
    *newNode = {data, left, right};

    return newNode;
}

void PrintTreeNodes(Node* root, FILE* output) {
    assert(root != nullptr);

    static int64_t rankArray[MAX_RECURSION_DEPTH] = {};
    static int32_t curRecursionDepth = 1;
    static int32_t curNodeNumber = 0;
    static Node* pointerAnchor = root;

    char dataConverted[MAX_NODE_DATA_LENGTH] = "";
    Convert1251ToUtf8((const char*)root->data, dataConverted);

    curNodeNumber++;    
    
    if (rankArray[curRecursionDepth] == 0) {
        rankArray[curRecursionDepth] = root - pointerAnchor;
    }
    else {
        fprintf(output, "\t{rank = same; %lld; %lld;}\n", rankArray[curRecursionDepth], root - pointerAnchor);
    }

    #if (_ENABLE_PTR_ADRESSES_VIEW_ == 1)
        fprintf(output, "\tsubgraph cluster%d {\n", curNodeNumber);
        fprintf(output, "\t\tstyle = filled;\n"
                        "\t\tcolor = lightgreen;\n"
                        "\t\t%lld;\n"
                        "\t\tlabel = \"%lld\";\n", root - pointerAnchor, root - pointerAnchor);
        fprintf(output, "\t}\n");
    #endif

    if ((root->left != nullptr) && (root->right != nullptr))  {
        fprintf(output, 
            "\t%lld[shape=plaintext, label = <"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING= \"0\" CELLPADDING=\"4\">"
            "<TR>"
                "<TD COLSPAN=\"2\">" TREE_TYPE "</TD>"
            "</TR>"
            "<TR>"
                "<TD PORT = \"l%d\">" LEFT_BRANCH "</TD>"
                "<TD PORT = \"r%d\">" RIGHT_BRANCH "</TD>"
            "</TR>"
            "</TABLE>>];\n",
            root - pointerAnchor, dataConverted, 
            curNodeNumber, LEFT_BRANCH_VALUE, 
            curNodeNumber, RIGHT_BRANCH_VALUE);

        curRecursionDepth += 1;

        fprintf(output, "\t\t%lld: <l%d> -> %lld[label = \"Да\"];\n", 
                root - pointerAnchor, curNodeNumber, root->left  - pointerAnchor);
        fprintf(output, "\t\t%lld: <r%d> -> %lld[label = \"Нет\"];\n", 
                root - pointerAnchor, curNodeNumber, root->right - pointerAnchor);

        PrintTreeNodes(root->left, output);
        PrintTreeNodes(root->right, output);
        curRecursionDepth--;
    }
    else {
        fprintf(output, 
            "\t%lld[shape = plaintext, label = <"
            "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING= \"0\" CELLPADDING=\"4\">"
            "<TR>"
                "<TD COLSPAN=\"2\">" TREE_TYPE "</TD>"
            "</TR>"
            "<TR>"
                "<TD> 0 </TD>"
                "<TD> 0 </TD>"
            "</TR>"
            "</TABLE>>];\n", 
            root - pointerAnchor, dataConverted);
    }
}

bool VerifyTree(Node* root) {
    assert(root != nullptr);

    if ((root->left == nullptr) != (root->right == nullptr)) {
        return 1;
    }

    if (root->left  != nullptr) return VerifyTree(root->left);
    if (root->right != nullptr) return VerifyTree(root->right);
    return 0;
}
