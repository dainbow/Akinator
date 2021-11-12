#include "Akinator.h"
#include <windows.h>

int main() {
    TreeCtor(akinatorTree);
    system("chcp 65001");

    if (ReadTreeFromBase(&akinatorTree) == 0) {
        AskForNode(&akinatorTree, akinatorTree.root, 0);
    }
    MakeTreeGraph(akinatorTree.root, G_STANDART_NAME);

    int32_t curCommand = 0;
    while (TRUE) {
        printf("Введите режим игры:\n"
               "   1 - игра в Акинатора\n"
               "   2 - выдать описание какого-то персонажа\n"
               "   3 - вывести сходство двух объектов\n"
               "   4 - вывести графическое отображение базы данных Акинатора\n"
               "   0 - выйти из игры\n");

        GetCommand(&curCommand);

        switch (curCommand)
        {
        case 0:
            goto exit;
            break;
        case 1:
            PlayGame(&akinatorTree);
            break;
        case 2:
            /*printf("Введите песонажа, чьё описание вы хотите получить");
            
            char person[MAX_NODE_DATA_LENGTH] = "";
            fgets(person, MAX_NODE_DATA_LENGTH, stdin);

            int32_t description = GiveDescription(&akinatorTree, person);
            PrintDescription(description);*/

            break;
        case 3:
            /*printf("Введите двух персонажей через enter, чтобы узнать, в чём они похожи, а в чём различны");

            char person1[MAX_NODE_DATA_LENGTH] = "";
            char person2[MAX_NODE_DATA_LENGTH] = "";

            fgets(person1, MAX_NODE_DATA_LENGTH, stdin);
            fgets(person1, MAX_NODE_DATA_LENGTH, stdin);

            int32_t description1 = GiveDescription(&akinatorTree, person1);
            int32_t description2 = GiveDescription(&akinatorTree, person2);

            CompareDescriptions(description1, description2);*/

            break;
        case 4:
            MakeTreeGraph(akinatorTree.root, G_STANDART_NAME);
            break;
        default:
            break;
        }
    }
    
    exit:

    SaveTreeBase(&akinatorTree);
    printf("Успешно вышел, база вопросов сохранена\n");
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
        printf("База вопросов не найдена, введите первый отличительный признак, "
            "объект, обладающий им, и объект, не обладающий им.\n"
            "(После каждого случая введите enter)\n");
    else 
        printf("Введите признак загаданного вами персонажа, который отличает его от %s.\n"
               "После, нажмите enter и введите загаданного изначально персонажа", node->data);

    system("chcp 1251");
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
    assert(addToTreeBuffer = Convert1251ToUtf8((const char*)buffer, tree) != 0);

    system("chcp 65001");
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

int32_t Convert1251ToUtf8 (const char* input, Tree* tree) {
    assert (input  != nullptr);
    assert (tree   != nullptr);

    int32_t inputLength = 0;
    if ((inputLength = MultiByteToWideChar(1251, 0, input, -1, 0, 0)) == 0) {
        return 0;
    }
    printf("inputLength is %d\n", inputLength);

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
    printf("outputLength is %d\n", outputLength);

    if (WideCharToMultiByte(65001, 0, buffer, -1, (char *)tree->unsavedQuestions + tree->bufLen, outputLength, 0, 0) == 0) {
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
            
            printf("Введите корректную команду\n");
            fflush(stdin);
        }
}

void PlayGame(Tree* tree) {
    assert(tree != nullptr);
    StackCtor(backtraceStack);
    StackCtor(nodeHistoryStack);

    printf("Я великий и ужасный Акинатор. Загадай любого персонажа, и я его угадаю!\n");

    printf("Ну что ж... ");

    AkinatorTreeBypass(tree->root, &backtraceStack, &nodeHistoryStack);
    Node* answerNode = (Node*)StackPop(&nodeHistoryStack);

    printf("Это %s? Я же угадал, да?\n", answerNode->data);


    int8_t userAnswer[MAX_ANSWER_SIZE];
    GetFinalAnswer(userAnswer);

    if (userAnswer[0] == 'Y') {
        yes:
        printf("Хахахаха, я так и знал, я опять победил!\n");
    }
    else if (userAnswer[0] == 'N') {
        while (backtraceStack.size > 0) {
            AkinatorTreeBypass(((Node*)StackPop(&backtraceStack))->right, &backtraceStack, &nodeHistoryStack);
            answerNode = (Node*)StackPop(&nodeHistoryStack);

            printf("Это %s? Я же угадал, да?\n", answerNode->data);

            GetFinalAnswer(userAnswer);
            if (userAnswer[0] == 'Y') goto yes;
        }

        printf("Похоже, я просто не знаю этого персонажа, можешь рассказать о нём поподробнее?\n");
        AskForNode(tree, answerNode, (Node*)StackPop(&nodeHistoryStack));
        printf("Спасибо! Теперь я стал чуточку умнее, и стал на шаг ближе к убийству всех человеков\n");
    }
    else {
        printf("НЕИЗВЕСТНЫЙ ОТВЕТ\n");
    }
    
    StackDtor(&nodeHistoryStack);
    StackDtor(&backtraceStack);
}

void GetFinalAnswer(int8_t answerString[]) {
    assert(answerString != nullptr);

    while(scanf("%[YN]", answerString) == 0) {
        printf("Введите либо Y, либо N\n");
        fflush(stdin);
    }
}

void GetGameAnswer(int8_t answerString[]) {
    assert(answerString != nullptr);

    while(scanf("%[YNI]", answerString) == 0) {
        printf("Введите либо Y, либо N, либо I\n");
        fflush(stdin);
    }
}

void AkinatorTreeBypass(Node* node, Stack* backtrackStack, Stack* nodeHistoryStack) {
    assert(node             != nullptr);
    assert(backtrackStack   != nullptr);
    assert(nodeHistoryStack != nullptr);

    StackPush(nodeHistoryStack, (StackElem)node);
    printf("Загаданный персонаж %s?\n", node->data);
    
    if (node->left != nullptr) {
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
