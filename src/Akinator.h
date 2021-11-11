#pragma once

#include <assert.h>
#include <locale.h>

#include "Text.h"
#include "Tree.h"
#include "Graph.h"
#include "Stack.h"

const char STANDART_BASE_LOCATION[] = "qdata/base.txt";
const int32_t MAX_TRASH_SIZE = 100;

void ScanBase(Text* input, Stack* stack);
Node* MakeTreeFromStack(Stack* nodesStack);
void ReadTreeFromBase(Tree* tree);
