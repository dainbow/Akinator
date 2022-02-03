#define TX_USE_SPEAK
#include "TXLib.h"

#include "Akinator.h"

int main() {
    system("chcp 1251");
    bool loop = 1;

    TreeCtor(akinatorTree);
    if (ReadTreeFromBase(&akinatorTree) == 0) {
        AskForNode(&akinatorTree, akinatorTree.root, 0);
    }

    while (loop) {
        printf("������� ����� ����:\n"
               "   1 - ���� � ���������\n"
               "   2 - ������ �������� ������-�� ���������\n"
               "   3 - ������� �������� ���� ��������\n"
               "   4 - ������� ����������� ����������� ���� ������ ���������\n"
               "   0 - ����� �� ����\n");

        int32_t curCommand = 0;
        GetCommand(&curCommand);

        switch (curCommand) {
            case 0:
                loop = 0;
                break;
            case 1: {
                PlayGame(&akinatorTree);
                break;
            }
            case 2: {
                PlayDescription(&akinatorTree);
                break;
            }
            case 3: {
                PlayCompartion(&akinatorTree);
                break;
            }
            case 4: {
                MakeTreeGraph(akinatorTree.root, G_STANDART_NAME);
                break;
            }
            default: {
                break;
            }
        }
    }

    SaveTreeBase(&akinatorTree);
        TreeDtor(&akinatorTree);
}

void ScanBase(Text* input, Stack* stack) {
    assert(input  != nullptr);
    assert(stack  != nullptr);

    int8_t trashBuff[MAX_TRASH_SIZE] = "";

    for (uint32_t curString = 0; curString < input->strAmount; curString++) {
        if (input->strings[curString].length == 1) {}
        else if (sscanf((const char*)input->strings[curString].value, "%*[{]%*[^{}]%[}]", trashBuff)) {
            StackPush(stack, (StackElem)0);
            StackPush(stack, input->strings[curString].value + 1);
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
        Node* newNode = MakeNewNode(StackPop(nodesStack));
        StackPush(&treeStack, (StackElem)newNode);

        if ((nodesStack->size > 0) && (nextLooker = StackPop(nodesStack))) {
            currentNode = (Node*)StackPop(&treeStack);

            currentNode->left   = (Node*)StackPop(&treeStack);
            currentNode->right  = (Node*)StackPop(&treeStack);

            StackPush(&treeStack, (StackElem)currentNode);

            currentNode = (Node*)StackPop(&treeStack);
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

bool ReadTreeFromBase(Tree* tree) {
    assert(tree != nullptr);
    StackCtor(nodesStack);

    if (ReadTextFromFile(&tree->qbase, STANDART_BASE_LOCATION) == 0)
        return 0;

    MakeStrings(&tree->qbase);
    ProcessStrings(&tree->qbase);

    ScanBase(&tree->qbase, &nodesStack);
    tree->root = MakeTreeFromStack(&nodesStack);

    StackDtor(&nodesStack);
    return 1;
}

void AskForNode(Tree* tree, Node* node, Node* preNode) {
    assert(tree != nullptr);
    assert(node != nullptr);

    if (tree->root == node)
        printf("���� �������� �� �������, ������� ������ ������������� �������, "
            "������, ���������� ��, � ������, �� ���������� ��.\n"
            "(����� ������� ������ ������� enter)\n");
    else
        printf("������� ����������� ���� ���������\n");

    Node* saveLastNode = 0;
    if (tree->root != node) {
        saveLastNode = node;
        node = MakeNewNode((TreeElem)(tree->unsavedQuestions + tree->bufLen));
    }

    fflush(stdin);
    node->left     = MakeNewNode((TreeElem)(tree->unsavedQuestions + tree->bufLen));
    tree->bufLen  += MyFGetsForOneItem(tree->unsavedQuestions + tree->bufLen, MAX_NODE_DATA_LENGTH, stdin);

    printf("��� �������� %s �� %s\n", node->left->data, saveLastNode->data);
    fflush(stdin);
    node->data     = tree->unsavedQuestions + tree->bufLen;
    tree->bufLen  += MyFGetsForOneItem(tree->unsavedQuestions + tree->bufLen, MAX_NODE_DATA_LENGTH, stdin);

    if (tree->root == node) {
        node->right    = MakeNewNode((TreeElem)(tree->unsavedQuestions + tree->bufLen));
        tree->bufLen  += MyFGetsForOneItem(tree->unsavedQuestions + tree->bufLen, MAX_NODE_DATA_LENGTH, stdin);
    }
    else {
        node->right = saveLastNode;
    }

    if (tree->root != node) {
        if (preNode->left == saveLastNode) {
            preNode->left = node;
        }

        if (preNode->right == saveLastNode) {
            preNode->right = node;
        }
    }
}

int32_t Convert1251ToUtf8 (const char* input, char* output) {
    assert (input    != nullptr);
    assert (output   != nullptr);

    int32_t inputLength = 0;
    if ((inputLength = MultiByteToWideChar(1251, 0, input, -1, 0, 0)) == 0) {
        return 0;
    }

    wchar_t* buffer = (wchar_t*)calloc(inputLength + 10, sizeof(buffer[0]));
    if (MultiByteToWideChar(1251, 0, input, -1, buffer, inputLength) == 0) {
        free(buffer);
        return 0;
    }

    int32_t outputLength = 0;
    if ((outputLength = WideCharToMultiByte(65001, 0, buffer, -1, 0, 0, 0, 0)) == 0) {
        free(buffer);
        return 0;
    }

    if (WideCharToMultiByte(65001, 0, buffer, -1, output, outputLength, 0, 0) == 0) {
        free(buffer);

        return 0;
    }
    free(buffer);
    return outputLength;
}

void GetCommand(int32_t* curCommand) {
    assert(curCommand != nullptr);

    while((scanf("%d", curCommand) == 0) ||
        !((*curCommand == 0) || (*curCommand == 1) || (*curCommand == 2) || (*curCommand == 3) || (*curCommand == 4))) {

            printf("����� ���� ����� 0 ��� 1 ��� 2 ��� 3 ��� 4, ����� ��� ��� ������?\n");
            fflush(stdin);
        }
}

void PlayGame(Tree* tree) {
    assert(tree != nullptr);
    StackCtor(backtraceStack);
    StackCtor(nodeHistoryStack);

    printf("� ������� � ������� ��������. ������� ������ ���������, � � ��� ������!\n");

    printf("�� ��� �... ");

    AkinatorTreeBypass(tree->root, &backtraceStack, &nodeHistoryStack);
    Node* answerNode = (Node*)StackPop(&nodeHistoryStack);

    printf("��� %s? � �� ������, ��?\n", answerNode->data);


    int8_t userAnswer[MAX_ANSWER_SIZE];
    GetFinalAnswer(userAnswer);

    if (userAnswer[0] == 'Y') {
        yes:
        printf("��������, � ��� � ����, � ����� �������!\n");
    }
    else if (userAnswer[0] == 'N') {
        while (backtraceStack.size > 0) {
            AkinatorTreeBypass(((Node*)StackPop(&backtraceStack))->right, &backtraceStack, &nodeHistoryStack);
            answerNode = (Node*)StackPop(&nodeHistoryStack);

            printf("��� %s? � �� ������, ��?\n", answerNode->data);

            GetFinalAnswer(userAnswer);
            if (userAnswer[0] == 'Y') goto yes;
        }

        printf("������, � ������ �� ���� ����� ���������, ������ ���������� � ��� �����������?\n");
        AskForNode(tree, answerNode, (Node*)StackPop(&nodeHistoryStack));
        printf("�������! ������ � ���� ������� �����, � ���� �� ��� ����� � �������� ���� ���������\n");
    }
    else {
        printf("����������� �����\n");
    }

    StackDtor(&nodeHistoryStack);
    StackDtor(&backtraceStack);
}

void GetFinalAnswer(int8_t answerString[]) {
    assert(answerString != nullptr);

    while(scanf("%[YN]", answerString) == 0) {
        printf("������� ���� Y, ���� N\n");
        fflush(stdin);
    }
}

void GetGameAnswer(int8_t answerString[]) {
    assert(answerString != nullptr);

    while(scanf("%[YNI]", answerString) == 0) {
        printf("������� ���� Y, ���� N, ���� I\n");
        fflush(stdin);
    }
}

void AkinatorTreeBypass(Node* node, Stack* backtrackStack, Stack* nodeHistoryStack) {
    assert(node             != nullptr);
    assert(backtrackStack   != nullptr);
    assert(nodeHistoryStack != nullptr);

    StackPush(nodeHistoryStack, (StackElem)node);

    if (node->left != nullptr) {
        printf("���������� �������� %s?\n", node->data);

        int8_t userAnswer[MAX_ANSWER_SIZE];
        GetGameAnswer(userAnswer);

        if (userAnswer[0] == 'Y') {
            AkinatorTreeBypass(node->left, backtrackStack, nodeHistoryStack);
        }
        else if (userAnswer[0] == 'N') {
            AkinatorTreeBypass(node->right, backtrackStack, nodeHistoryStack);
        }
        else if (userAnswer[0] == 'I') {
            StackPush(backtrackStack, (StackElem)node);
            AkinatorTreeBypass(node->left, backtrackStack, nodeHistoryStack);
        }
    }
}

void SaveTreeBase(Tree* tree) {
    assert(tree != nullptr);
    FILE* output = fopen(STANDART_BASE_LOCATION, "w");

    PrintNodeToBuffer(tree->root, output);

    fclose(output);
}

void PrintNodeToBuffer(Node* node, FILE* output) {
    assert(node   != nullptr);

    if (node->left != nullptr) {
        fprintf(output, "{\n"
                        "%s\n", node->data);
        PrintNodeToBuffer(node->left,  output);
        PrintNodeToBuffer(node->right, output);
        fprintf(output, "}\n");
    }
    else {
        fprintf(output, "{%s}\n", node->data);
    }
}

void GiveDescription(Node* node, int8_t person[], Stack* stack, bool *isFoundFlag) {
    assert(node   != nullptr);
    assert(person != nullptr);

    if (*isFoundFlag) {
        return;
    }
    else {
        StackPush(stack, (StackElem)node);
    }

    if (!strcmp((const char*)node->data, (const char*)person)) {
        *isFoundFlag = 1; 
        return;
    }

    if ((node->left  != nullptr) &&
        (node->right != nullptr)) {
        GiveDescription(node->left,  person, stack, isFoundFlag);
        GiveDescription(node->right, person, stack, isFoundFlag);
    }

    if (!(*isFoundFlag)) {
        StackPop(stack);
    }
}

void PrintDescription (Stack* stack, int8_t person[]) {
    assert(stack  != nullptr);
    assert(person != nullptr);

    printf("����, %s ", person);
    for (int32_t curIdx = 0; curIdx < stack->size - 1; curIdx++) {
        Node* currentNode = (Node*)StackPopIndexDEVELOPERS_ONLY(stack, curIdx);
        Node* nextNode    = (Node*)StackPopIndexDEVELOPERS_ONLY(stack, curIdx + 1);

        if (currentNode->left == nextNode) {
            printf("%s; ", currentNode->data);
        }
        else {
            printf("�� %s; ", currentNode->data);
        }

        StackPushIndexDEVELOPERS_ONLY(stack, (StackElem)nextNode, curIdx + 1);
    }

    stack->size = 0;
    printf("\n");
}

void PrintDescriptionComparation(Stack* stack1, Stack* stack2, int8_t person1[], int8_t person2[]) {
    assert(stack1  != nullptr);
    assert(stack2  != nullptr);
    assert(person1 != nullptr);
    assert(person2 != nullptr);

    printf("����, %s � %s: ", person1, person2);
    for (int32_t curIdx = 0; (curIdx < stack1->size - 1) && (curIdx < stack2->size - 1);
                                                                                        curIdx++) {
        Node* currentNode1 = (Node*)StackPopIndexDEVELOPERS_ONLY(stack1, curIdx);
        Node* nextNode1    = (Node*)StackPopIndexDEVELOPERS_ONLY(stack1, curIdx + 1);

        Node* currentNode2 = (Node*)StackPopIndexDEVELOPERS_ONLY(stack2, curIdx);
        Node* nextNode2    = (Node*)StackPopIndexDEVELOPERS_ONLY(stack2, curIdx + 1);

        if ((currentNode1 == currentNode2) && (nextNode1 == nextNode2)) {
            if (currentNode1->left == nextNode1) {
                printf("��� %s; ",   currentNode1->data);
            }
            else {
                printf("��� �� %s; ", currentNode1->data);
            }
        }
        else {
            if (currentNode1->left == nextNode1) {
                printf("%s %s, � ", person1, currentNode1->data);
            }
            else {
                printf("%s �� %s, � ", person1, currentNode1->data);
            }

            if (currentNode2->left == nextNode2) {
                printf("%s %s; ", person2, currentNode2->data);
            }
            else {
                printf("%s �� %s; ", person2, currentNode2->data);
            }
        }

        StackPushIndexDEVELOPERS_ONLY(stack1, (StackElem)nextNode1, curIdx + 1);
        StackPushIndexDEVELOPERS_ONLY(stack2, (StackElem)nextNode2, curIdx + 1);
    }
    
    printf("\n");
    if (stack1->size != stack2->size) {
        Stack* biggerStack = 0;
        Stack* littleStack = 0;

        if (stack1->size > stack2->size) {
            biggerStack = stack1;
            littleStack = stack2;

            printf("�� � %s �����: ", person1);
        }
        else {
            biggerStack = stack2;
            littleStack = stack1;

            printf("�� � %s �����: ", person2);
        }

        for (int32_t curIdx = littleStack->size - 1; curIdx < biggerStack->size - 1; curIdx++) {
            Node* currentNode = (Node*)StackPopIndexDEVELOPERS_ONLY(biggerStack, curIdx);
            Node* nextNode    = (Node*)StackPopIndexDEVELOPERS_ONLY(biggerStack, curIdx + 1);

            if (currentNode->left == nextNode) {
                printf("%s; ", currentNode->data);
            }
            else {
                printf("�� %s; ", currentNode->data);
            }

            StackPushIndexDEVELOPERS_ONLY(biggerStack, (StackElem)nextNode, curIdx + 1);
        }

        stack1->size = 0;
        stack2->size = 0;
        printf("\n");
    }
}

void PlayDescription(Tree* akinatorTree) {
    assert(akinatorTree != nullptr);

    printf("������� ��������, ��� �������� �� ������ ��������\n");
    StackCtor(descriptionStack);
    bool flag = 0;

    int8_t person[MAX_NODE_DATA_LENGTH]    = "";

    fflush(stdin);
    MyFGetsForOneItem(person, MAX_NODE_DATA_LENGTH, stdin);

    GiveDescription(akinatorTree->root, person, &descriptionStack, &flag);
    if (!flag) {
        printf("�� ��� ��������������� ���������, ����� �������\n");
    }
    else {
        PrintDescription(&descriptionStack, person);
    }

    StackDtor(&descriptionStack);
}

void PlayCompartion(Tree* akinatorTree) {
    assert(akinatorTree != nullptr);
    
    printf("������� ���� ���������� ����� enter, ����� ������, � ��� ��� ������, � � ��� ��������\n");
    StackCtor(descriptionStack1);
    StackCtor(descriptionStack2);

    bool flag1 = 0;
    bool flag2 = 0;

    int8_t person1[MAX_NODE_DATA_LENGTH] = "";
    int8_t person2[MAX_NODE_DATA_LENGTH] = "";

    fflush(stdin);
    MyFGetsForOneItem(person1, MAX_NODE_DATA_LENGTH, stdin);
    GiveDescription(akinatorTree->root, person1, &descriptionStack1, &flag1);

    fflush(stdin);
    MyFGetsForOneItem(person2, MAX_NODE_DATA_LENGTH, stdin);
    GiveDescription(akinatorTree->root, person2, &descriptionStack2, &flag2);

    if (!flag1 || !flag2) {
        printf("�����-�� ��������, �� ���, ������� �� ���, �� ����������, ����� ������\n");
    }
    else {
        PrintDescriptionComparation(&descriptionStack1, &descriptionStack2, person1, person2);
    }

    StackDtor(&descriptionStack1);
    StackDtor(&descriptionStack2);
}
