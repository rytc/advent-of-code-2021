#include "../utils/utils.h"
#include <cassert>

struct Node {
    u8 code; 
    Node* parent;
    u8 col;
};

char openings[] = {'{', '[', '<', '('};
char closings[] = {'}', ']', '>', ')'};

static u8
get_code(char c) {
    if(c == '{' || c == '}') return 0;
    if(c == '[' || c == ']') return 1;
    if(c == '<' || c == '>') return 2;
    if(c == '(' || c == ')') return 3;

    assert(!"should not get here!");
    return 255;
}

static u32 
get_first_error_score(char* line) {
    char* c = line;

    Node* nodePool = (Node*)malloc(128 * sizeof(Node));
    Node* lastNode = nullptr;
    u32 nodeCount = 0;

    u32 firstIllegalScore = 0;
    u32 counter = 0;
    while(*c != '\n') {
        char code = *c;
        if(code == '{' || code == '[' || code == '<' || code == '(') {
            Node* newNode = &nodePool[nodeCount];
            newNode->code = code;
            newNode->parent = lastNode;
            newNode->col = counter;
            nodeCount++;

            lastNode = newNode;
        } else if(code == '}' || code == ']' || code == '>' || code == ')') {
            if(get_code(lastNode->code) != get_code(code)) {
                printf(">> at %i - expected %c got %c. Opening at %i. ", counter, closings[get_code(lastNode->code)], code, lastNode->col);
                switch(code) {
                    case '}': firstIllegalScore = 1197;  break;
                    case ']': firstIllegalScore = 57;    break;
                    case '>': firstIllegalScore = 25137; break;
                    case ')': firstIllegalScore = 3;     break;
                }    
                printf("- Score: %i\n", firstIllegalScore);
                printf("  %s", line);
                break;
            } 

            lastNode = lastNode->parent;
            nodeCount--;
        }

        counter++;
        c++;
    }


    free(nodePool);
    return firstIllegalScore;
}



int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 lineCount = count_newlines(input);

    u32 totalErrorScore = 0;
    for(u32 i = 0; i < lineCount; i++) {
        char line[128] = {};
        u32 end = find_next(input, '\n');
        u32 len = (end - input.cursor);
        memcpy(line, input.data + input.cursor, len + 1);
        u32 errorScore = get_first_error_score(line);

        if(errorScore == 0) { 
            printf("No error found on line %i! With length %i\n", i, len);
        } else {
            printf("<< Error on line %i with len %i\n", i, len);
        }
        totalErrorScore += errorScore;
        input.cursor = end + 1;
    }

    printf("Total error score: %i\n", totalErrorScore);

    free_file(input);
    return 0;
}