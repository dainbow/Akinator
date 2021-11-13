#pragma once

#include <assert.h>
#include <locale.h>

#include "Text.h"
#include "Tree.h"
#include "Graph.h"
#include "Stack.h"
#include "Utilities.h"

const char STANDART_BASE_LOCATION[] = "qdata/base.txt";
const int32_t MAX_TRASH_SIZE        = 100;
const int32_t MAX_NODE_DATA_LENGTH  = 100;
const int32_t MAX_ANSWER_SIZE       = 10;

void ScanBase(Text* input, Stack* stack);
Node* MakeTreeFromStack(Stack* nodesStack);
bool ReadTreeFromBase(Tree* tree);
void AskForNode(Tree* tree, Node* node, Node* preNode);

int32_t Convert1251ToUtf8 (const char* input, char* output);

void GetCommand(int32_t* curCommand);
void GetFinalAnswer(int8_t answerString[]);
void GetGameAnswer(int8_t answerString[]);

void PlayGame(Tree* tree);
void PlayDescription(Tree* akinatorTree);
void PlayCompartion(Tree* akinatorTree);

void AkinatorTreeBypass(Node* node, Stack* backtrackStack, Stack* nodeHistoryStack);

void SaveTreeBase(Tree* tree);
void PrintNodeToBuffer(Node* node, FILE* output);

void GiveDescription(Node* node, int8_t person[], Stack* stack);
void PrintDescription (Stack* stack, int8_t person[]);
void PrintDescriptionComparation(Stack* stack1, Stack* stack2, int8_t person1[], int8_t person2[]);
