#include "Akinator.h"

int main() {
    TreeCtor(akinatorTree);

    ReadTreeFromBase(&akinatorTree);
    MakeTreeGraph(akinatorTree.root, G_STANDART_NAME);
    
    TreeDtor(&akinatorTree);
}

void ScanBase(Text* input, Stack* stack) {
    assert(input  != nullptr);
    assert(stack  != nullptr);

    int8_t trashBuff[MAX_TRASH_SIZE] = "";

    for (uint32_t curString = 0; curString < input->strAmount; curString++) {
        if (input->strings[curString].length == 1) {}
        else if (sscanf((const char*)input->strings[curString].value, "%*[{]%*[^{}]%[}]", trashBuff)) {
            StackPush(stack, 0);
            StackPush(stack, input->strings[curString].value);
        }
        else {
            StackPush(stack, input->strings[curString].value);
        }
    }
}

Node* MakeTreeFromStack(Stack* nodesStack) {
    assert(nodesStack != nullptr);
    assert(nodesStack->size > 0);

    StackCtor(treeStack);
    Node* currentNode = 0;
    StackElem nextLooker = 0;

    while (nodesStack->size) {
        StackPush(&treeStack, (StackElem)MakeNewNode(StackPop(nodesStack)));
        
        if ((nodesStack->size > 0) && (nextLooker = StackPop(nodesStack))) {
            currentNode = (Node*)StackPop(&treeStack);

            currentNode->left = (Node*)StackPop(&treeStack);
            currentNode->right  = (Node*)StackPop(&treeStack);

            StackPush(&treeStack, (StackElem)currentNode);

            currentNode = (Node*)StackPop(&treeStack);
            wprintf(L"Pushed node {%s; %s; %s}\n", currentNode->data, currentNode->left->data, currentNode->right->data);
            StackPush(&treeStack, (StackElem)currentNode);

            StackPush(nodesStack, nextLooker);
        }
    }
    currentNode = (Node*)StackPop(&treeStack);
    currentNode->left = (Node*)StackPop(&treeStack);
    currentNode->right  = (Node*)StackPop(&treeStack);
    StackPush(&treeStack, (StackElem)currentNode);

    currentNode = (Node*)StackPop(&treeStack);
    StackDtor(&treeStack);

    return currentNode;
}

void ReadTreeFromBase(Tree* tree) {
    assert(tree != nullptr);
    StackCtor(nodesStack);

    ReadTextFromFile(&tree->qbase, STANDART_BASE_LOCATION);
    MakeStrings(&tree->qbase);   
    ProcessStrings(&tree->qbase);

    ScanBase(&tree->qbase, &nodesStack);
    tree->root = MakeTreeFromStack(&nodesStack);

    StackDtor(&nodesStack);
}
