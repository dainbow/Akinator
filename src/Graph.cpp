#include "Graph.h"
#include "Tree.h"

char* MakeTreeGraph(Node* tree, const char name[]) {
    assert(tree != nullptr);

    static char endName[MAX_PATH_FILE_LENGTH] = "";

    GenerateOutputName(name, endName, G_PATH, G_OUTPUT_FORMAT);

    FILE* output = fopen(endName, "w");
    assert(output != nullptr);

    fprintf(output, "digraph G{ \n");
    fprintf(output, "\tnewrank=true;\n");
    PrintTreeNodes(tree, output);
    fprintf(output, "}\n");

    char command[MAX_COMMAND_NAME] = "";
    char del[MAX_COMMAND_NAME]     = "";
    fclose(output);

    sprintf(command, "%s %s -Tpng -O", 
            DOT_EXE_PATH, endName);
    sprintf(del, "del %s", endName);

    system(command);
    //system(del);

    return endName;

}

void GenerateOutputName(const char beginningName[], char endName[], const char path[], const char format[]) {
    assert(beginningName != nullptr);
    assert(endName != nullptr);

    uint32_t fileCopyAmount = 0;
    int32_t fileD = 0;

    sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);
    while ((fileD = open(endName, O_RDONLY)) != -1) {
        fileCopyAmount++;
        sprintf(endName, "%s%s(%u)%s", path, beginningName, fileCopyAmount, format);
    }
    close(fileD);
    sprintf(endName, "%s%s(%u)", path, beginningName, fileCopyAmount);
}
