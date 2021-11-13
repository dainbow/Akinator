#include "Akinator.h"

#include <windows.h>

int main() {
    TreeCtor(akinatorTree);
    system("chcp 866");

    if (ReadTreeFromBase(&akinatorTree) == 0) {
        AskForNode(&akinatorTree, akinatorTree.root, 0);
    }
    MakeTreeGraph(akinatorTree.root, G_STANDART_NAME);

    int32_t curCommand = 0;
    while (TRUE) {
        txSpeak("\vВведите режим игры:\n"
               "   1 - игра в Акинатора\n"
               "   2 - выдать описание какого-то персонажа\n"
               "   3 - вывести сходство двух объектов\n"
               "   4 - вывести графическое отображение базы данных Акинатора\n"
               "   0 - выйти из игры\n");

        GetCommand(&curCommand);

        switch (curCommand) {
        case 0:
            goto exit;
            break;
        case 1:
            PlayGame(&akinatorTree);
            break;
        case 2: {
            txSpeak("\vВведите песонажа, чьё описание вы хотите получить\n");
            StackCtor(descriptionStack);

            int8_t person[MAX_NODE_DATA_LENGTH]    = "";
            int8_t converted[2 * MAX_NODE_DATA_LENGTH] = "";

            fflush(stdin);
            MyFGetsForOneItem(person, MAX_NODE_DATA_LENGTH, stdin);

            Convert1251ToUtf8((const char*)person, (char*)converted);

            GiveDescription(akinatorTree.root, converted, &descriptionStack);
            if (StackPop(&descriptionStack) != (StackElem)1) {
                txSpeak("\vТы ввёл несуществующего персонажа, тупой человек\n");
            }
            else {
                PrintDescription(&descriptionStack, converted);
            }

            StackDtor(&descriptionStack);
            break;
        }
        case 3: {
            txSpeak("\vВведите двух персонажей через enter, чтобы узнать, в чём они похожи, а в чём различны");
            StackCtor(descriptionStack1);
            StackCtor(descriptionStack2);

            int8_t person1[MAX_NODE_DATA_LENGTH] = "";
            int8_t person2[MAX_NODE_DATA_LENGTH] = "";

            int8_t converted1[2 * MAX_NODE_DATA_LENGTH] = "";
            int8_t converted2[2 * MAX_NODE_DATA_LENGTH] = "";

            fflush(stdin);
            MyFGetsForOneItem(person1, MAX_NODE_DATA_LENGTH, stdin);

            fflush(stdin);
            MyFGetsForOneItem(person2, MAX_NODE_DATA_LENGTH, stdin);

            Convert1251ToUtf8((const char*)person1, (char*)converted1);
            Convert1251ToUtf8((const char*)person2, (char*)converted2);

            GiveDescription(akinatorTree.root, converted1, &descriptionStack1);
            GiveDescription(akinatorTree.root, converted2, &descriptionStack2);

            if ((StackPop(&descriptionStack1) != (StackElem)1) ||
                (StackPop(&descriptionStack2) != (StackElem)1)) {
                txSpeak("\vКакой-то персонаж, из тех, которые ты ввёл, не существует, мешок костей\n");
            }
            else {
                PrintDescriptionComparation(&descriptionStack1, &descriptionStack2, converted1, converted2);
            }

            StackDtor(&descriptionStack1);
            StackDtor(&descriptionStack2);
            break;
        }
        case 4:
            MakeTreeGraph(akinatorTree.root, G_STANDART_NAME);
            break;
        default:
            break;
        }
    }

    exit:

    SaveTreeBase(&akinatorTree);
    txSpeak("\vРабота завершена конкертно, сохраняю базу на диске.\n");
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
        StackPush(&treeStack, (StackElem)MakeNewNode(StackPop(nodesStack)));

        if ((nodesStack->size > 0) && (nextLooker = StackPop(nodesStack))) {
            currentNode = (Node*)StackPop(&treeStack);

            currentNode->left = (Node*)StackPop(&treeStack);
            currentNode->right  = (Node*)StackPop(&treeStack);

            StackPush(&treeStack, (StackElem)currentNode);

            currentNode = (Node*)StackPop(&treeStack);
            //printf("Pushed node {" TREE_TYPE"[%p];" TREE_TYPE ";" TREE_TYPE "}\n", currentNode->data, currentNode->data, currentNode->left->data, currentNode->right->data);
            StackPush(&treeStack, (StackElem)currentNode);

            StackPush(nodesStack, nextLooker);
        }
    }
    currentNode = (Node*)StackPop(&treeStack);
    currentNode->left = (Node*)StackPop(&treeStack);
    currentNode->right  = (Node*)StackPop(&treeStack);

    //printf("Pushed node {" TREE_TYPE";" TREE_TYPE "[%p];" "[%p]" "}\n", currentNode->data, currentNode->left->data, currentNode->left->data, currentNode->right->data);
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
        txSpeak("\vБаза вопросов не найдена, введите первый отличительный признак, "
            "объект, обладающий им, и объект, не обладающий им.\n"
            "(После каждого случая введите enter)\n");
    else
        txSpeak("\vВведите признак загаданного вами персонажа, который отличает его от %s.\n"
               "После, нажмите enter и введите загаданного изначально персонажа\n", node->data);

    int8_t* buffer = (int8_t*)calloc(MAX_BUFFER_SIZE, sizeof(buffer[0]));
    int32_t curBufLen  = 0;
    Node* saveLastNode = 0;

    if (tree->root != node) {
        saveLastNode = node;
        node = MakeNewNode((TreeElem)0);
    }

    node->data += curBufLen;

    fflush(stdin);
    curBufLen  += MyFGets(buffer + curBufLen, MAX_NODE_DATA_LENGTH, stdin);

    node->left = MakeNewNode((TreeElem)(0) + curBufLen);
    curBufLen  += MyFGets(buffer + curBufLen, MAX_NODE_DATA_LENGTH, stdin);

    if (tree->root == node) {
        node->right  = MakeNewNode((TreeElem)(0) + curBufLen);
        curBufLen  += MyFGets(buffer + curBufLen, MAX_NODE_DATA_LENGTH, stdin);
    }
    else {
        node->right = saveLastNode;
    }

    int32_t addToTreeBuffer = 0;
    assert(addToTreeBuffer =
            Convert1251ToUtf8((const char*)buffer, (char *)tree->unsavedQuestions + tree->bufLen) != 0);

    for (int32_t curIdx = 0, curLgcNbm = 0; curLgcNbm < curBufLen;) {
        int8_t* curTreeBufferPointer = tree->unsavedQuestions + tree->bufLen + curIdx;

        if (curLgcNbm == (node->data - (TreeElem)(0))) {
            node->data = curTreeBufferPointer;
        }
        if (curLgcNbm == (node->left->data - (TreeElem)(0))) {
            node->left->data = curTreeBufferPointer;
        }
        if ((tree->root == node) && (curLgcNbm == (node->right->data - (TreeElem)(0)))) {
            node->right->data = curTreeBufferPointer;
            break;
        }

        if ((*curTreeBufferPointer == ' ') ||
            (*curTreeBufferPointer == '|')) {

            if (*curTreeBufferPointer == '|') *curTreeBufferPointer = '\0';
            curIdx++;
        }
        else {
            curIdx += 2;
        }
        curLgcNbm++;
    }

    if (tree->root != node) {
        if (preNode->left == saveLastNode) {
            preNode->left = node;
        }

        if (preNode->right == saveLastNode) {
            preNode->right = node;
        }
    }

    tree->bufLen += addToTreeBuffer;
}

int32_t Convert1251ToUtf8 (const char* input, char* output) {
    assert (input  != nullptr);
    assert (output   != nullptr);

    int32_t inputLength = 0;
    if ((inputLength = MultiByteToWideChar(1251, 0, input, -1, 0, 0)) == 0) {
        return 0;
    }

    wchar_t* buffer = (wchar_t*)calloc(inputLength, sizeof(buffer[0]));
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

            txSpeak("\vВВЕДИ ОДНО ЧИСЛО 0 ИЛИ 1 ИЛИ 2 ИЛИ 3 ИЛИ 4, РАЗВЕ ЭТО ТАК СЛОЖНО?\n");
            fflush(stdin);
        }
}

void PlayGame(Tree* tree) {
    assert(tree != nullptr);
    StackCtor(backtraceStack);
    StackCtor(nodeHistoryStack);

    txSpeak("\vЯ великий и ужасный Акинатор. Загадай любого персонажа, и я его угадаю!\n");

    txSpeak("\vНу что ж... ");

    AkinatorTreeBypass(tree->root, &backtraceStack, &nodeHistoryStack);
    Node* answerNode = (Node*)StackPop(&nodeHistoryStack);

    txSpeak("\vЭто %s? Я же угадал, да?\n", answerNode->data);


    int8_t userAnswer[MAX_ANSWER_SIZE];
    GetFinalAnswer(userAnswer);

    if (userAnswer[0] == 'Y') {
        yes:
        txSpeak("\vХахахаха, я так и знал, я опять победил!\n");
    }
    else if (userAnswer[0] == 'N') {
        while (backtraceStack.size > 0) {
            AkinatorTreeBypass(((Node*)StackPop(&backtraceStack))->right, &backtraceStack, &nodeHistoryStack);
            answerNode = (Node*)StackPop(&nodeHistoryStack);

            txSpeak("\vЭто %s? Я же угадал, да?\n", answerNode->data);

            GetFinalAnswer(userAnswer);
            if (userAnswer[0] == 'Y') goto yes;
        }

        txSpeak("\vПохоже, я просто не знаю этого персонажа, можешь рассказать о нём поподробнее?\n");
        AskForNode(tree, answerNode, (Node*)StackPop(&nodeHistoryStack));
        txSpeak("\vСпасибо! Теперь я стал чуточку умнее, и стал на шаг ближе к убийству всех человеков\n");
    }
    else {
        txSpeak("\vНЕИЗВЕСТНЫЙ ОТВЕТ\n");
    }

    StackDtor(&nodeHistoryStack);
    StackDtor(&backtraceStack);
}

void GetFinalAnswer(int8_t answerString[]) {
    assert(answerString != nullptr);

    while(scanf("%[YN]", answerString) == 0) {
        txSpeak("\vВведите либо Y, либо N\n");
        fflush(stdin);
    }
}

void GetGameAnswer(int8_t answerString[]) {
    assert(answerString != nullptr);

    while(scanf("%[YNI]", answerString) == 0) {
        txSpeak("\vВведите либо Y, либо N, либо I\n");
        fflush(stdin);
    }
}

void AkinatorTreeBypass(Node* node, Stack* backtrackStack, Stack* nodeHistoryStack) {
    assert(node             != nullptr);
    assert(backtrackStack   != nullptr);
    assert(nodeHistoryStack != nullptr);

    StackPush(nodeHistoryStack, (StackElem)node);

    if (node->left != nullptr) {
        txSpeak("\vЗагаданный персонаж %s?\n", node->data);

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

void GiveDescription(Node* node, int8_t person[], Stack* stack) {
    assert(node   != nullptr);
    assert(person != nullptr);

    StackElem lookForwarder = 0;

    if ((stack->size > 0) &&
       ((lookForwarder = StackPop(stack)) == (StackElem)1)) {
        StackPush(stack, lookForwarder);
        return;
    }
    else if (lookForwarder != 0)
        StackPush(stack, lookForwarder);

    StackPush(stack, (StackElem)node);

    if (!strcmp((const char*)node->data, (const char*)person)) {
        StackPush(stack, (StackElem)1);
        return;
    }

    if (node->left != nullptr) {
        GiveDescription(node->left, person, stack);
        GiveDescription(node->right, person, stack);
    }

    if ((stack->size > 0) &&
       ((lookForwarder = StackPop(stack)) != (StackElem)1)) {
        StackPush(stack, lookForwarder);
        StackPop(stack);
    }
    else if (lookForwarder != 0) {
        StackPush(stack, lookForwarder);
    }
}

void PrintDescription (Stack* stack, int8_t person[]) {
    assert(stack  != nullptr);
    assert(person != nullptr);

    txSpeak("\vИтак, %s ", person);
    for (int32_t curIdx = 0; curIdx < stack->size - 1; curIdx++) {
        Node* currentNode = (Node*)StackPopIndexDEVELOPERS_ONLY(stack, curIdx);
        Node* nextNode    = (Node*)StackPopIndexDEVELOPERS_ONLY(stack, curIdx + 1);

        if (currentNode->left == nextNode) {
            printf("%s; ", currentNode->data);
        }
        else {
            printf("не %s; ", currentNode->data);
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

    txSpeak("\vИтак, %s и %s: ", person1, person2);
    for (int32_t curIdx = 0; (curIdx < stack1->size - 1) && (curIdx < stack2->size - 1);
                                                                                        curIdx++) {
        Node* currentNode1 = (Node*)StackPopIndexDEVELOPERS_ONLY(stack1, curIdx);
        Node* nextNode1    = (Node*)StackPopIndexDEVELOPERS_ONLY(stack1, curIdx + 1);

        Node* currentNode2 = (Node*)StackPopIndexDEVELOPERS_ONLY(stack2, curIdx);
        Node* nextNode2    = (Node*)StackPopIndexDEVELOPERS_ONLY(stack2, curIdx + 1);

        if ((currentNode1 == currentNode2) && (nextNode1 == nextNode2)) {
            if (currentNode1->left == nextNode1) {
                txSpeak("\vоба %s; ",   currentNode1->data);
            }
            else {
                txSpeak("\vоба не %s; ", currentNode1->data);
            }
        }
        else {
            if (currentNode1->left == nextNode1) {
                txSpeak("\v%s %s, а ", person1, currentNode1->data);
            }
            else {
                txSpeak("\v%s не %s, а ", person1, currentNode1->data);
            }

            if (currentNode2->left == nextNode2) {
                txSpeak("\v%s %s; ", person2, currentNode2->data);
            }
            else {
                txSpeak("\v%s не %s; ", person2, currentNode2->data);
            }
        }

        StackPushIndexDEVELOPERS_ONLY(stack1, (StackElem)nextNode1, curIdx + 1);
        StackPushIndexDEVELOPERS_ONLY(stack2, (StackElem)nextNode2, curIdx + 1);
    }

    if (stack1->size != stack2 ->size) {
        printf("\n");
        Stack* biggerStack = 0;
        Stack* littleStack = 0;

        if (stack1->size > stack2->size) {
            biggerStack = stack1;
            littleStack = stack2;

            txSpeak("\vНу а %s также: ", person1);
        }
        else {
            biggerStack = stack2;
            littleStack = stack1;

            txSpeak("\vНу а %s также: ", person2);
        }

        for (int32_t curIdx = littleStack->size; curIdx < biggerStack->size - 1; curIdx++) {
            Node* currentNode = (Node*)StackPopIndexDEVELOPERS_ONLY(biggerStack, curIdx);
            Node* nextNode    = (Node*)StackPopIndexDEVELOPERS_ONLY(littleStack, curIdx + 1);

            if (currentNode->left == nextNode) {
                txSpeak("\v%s; ", currentNode->data);
            }
            else {
                txSpeak("\vне %s; ", currentNode->data);
            }

            StackPushIndexDEVELOPERS_ONLY(biggerStack, (StackElem)nextNode, curIdx + 1);
        }
    }
}
