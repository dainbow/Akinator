#pragma once

#include <fcntl.h>
#include <sys\types.h>
#include <io.h>

#include "Tree.h"

const char G_PATH[] = "graphs\\";
const char G_OUTPUT_FORMAT[] = ".png";
const char DOT_EXE_PATH[] = "\"C:\\Program Files\\Graphviz\\bin\\dot.exe\"";

const int32_t MAX_PATH_FILE_LENGTH = 100;
const int32_t MAX_COMMAND_NAME     = 100;

char* MakeTreeGraph(Node* tree, const char name[]);
void GenerateOutputName(const char beginningName[], char endName[], const char path[], const char format[]);
