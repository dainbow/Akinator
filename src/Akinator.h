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
const int32_t MAX_OUTPUT_BASE_SIZE  = 10000;

void ScanBase(Text* input, Stack* stack);
Node* MakeTreeFromStack(Stack* nodesStack);
bool ReadTreeFromBase(Tree* tree);
void AskForNode(Tree* tree, Node* node, Node* preNode);

int32_t Convert1251ToUtf8 (const char* input, Tree* tree);

void GetCommand(int32_t* curCommand);
void GetFinalAnswer(int8_t answerString[]);
void GetGameAnswer(int8_t answerString[]);

void PlayGame(Tree* tree);
void AkinatorTreeBypass(Node* node, Stack* backtrackStack, Stack* nodeHistoryStack);

void SaveTreeBase(Tree* tree);
void PrintNodeToBuffer(Node* node, FILE* output);
