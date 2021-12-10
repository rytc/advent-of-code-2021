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
            if(get_code(lastNode->code) != get_code(code) && firstIllegalScore == 0) {
                printf(">> at %i - expected %c got %c. Opening at %i. ", counter, closings[get_code(lastNode->code)], code, lastNode->col);
                switch(code) {
                    case '}': firstIllegalScore = 1197;  break;
                    case ']': firstIllegalScore = 57;    break;
                    case '>': firstIllegalScore = 25137; break;
                    case ')': firstIllegalScore = 3;     break;
                }    
                printf("- Score: %i\n", firstIllegalScore);
                printf("  %s", line);
            } 

            lastNode = lastNode->parent;
            nodeCount--;
        }

        counter++;
        c++;
    }

    if(nodeCount != 0) {
        printf(">> Incomplete line, %i nodes remain\n", nodeCount);
        for(u32 i = 0; i < nodeCount; i++) {
            printf(" %c", nodePool[i].code);
        }
        printf("\n");
    } else {
        printf("!! Complete line!\n");
    }

    free(nodePool);
    return firstIllegalScore;
}

static u64 
autocomplete(char* line) {
    char* c = line;

    Node* nodePool = (Node*)malloc(128 * sizeof(Node));
    Node* lastNode = nullptr;
    u32 nodeCount = 0;

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
                printf("Unexpected corrupted line!\n");
            } 

            lastNode = lastNode->parent;
            nodeCount--;
        }

        counter++;
        c++;
    }

    assert(nodeCount < 16);
    assert(nodeCount > 0);


    u64 autocompleteScore = 0;
    char closingList[16] = {};

    for(u32 i = 0; i < nodeCount; i++) {
        closingList[i] = closings[get_code(nodePool[i].code)];
    }

    for(s32 i = nodeCount-1; i >= 0; i--) {
    //for(u32 i = 0; i < nodeCount; i++) {
        u32 codeScore = 0;
        if(closingList[i] == ')') codeScore = 1;
        if(closingList[i] == ']') codeScore = 2;
        if(closingList[i] == '}') codeScore = 3;
        if(closingList[i] == '>') codeScore = 4;
        autocompleteScore = (autocompleteScore * 5) + codeScore;
    }
        
    free(nodePool);
    return autocompleteScore;
}

//
// Don't mind my lovely o(n^n) sort :|
//
static void
sort_scores(u64* scores, u32 count) {
    for(u32 i = 0; i < count; i++) {
        for(u32 j = 0; j < count; j++) {
            if(scores[j] > scores[i]) {
                u64 s = scores[j];
                scores[j] = scores[i];
                scores[i] = s;
            }
        }
    }
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 lineCount = count_newlines(input);
    char* incompleteLinesOnly[100] = {};
    u32 incompleteLineCount = 0;

    u32 totalErrorScore = 0;
    for(u32 i = 0; i < lineCount; i++) {
        char line[128] = {};
        u32 end = find_next(input, '\n');
        u32 len = (end - input.cursor);
        memcpy(line, input.data + input.cursor, len + 1);
        u32 errorScore = get_first_error_score(line);

        // Seems like every single line in the input data is incomplete, so we just 
        // assume it's incomplete, and if there is no error score then it's not corrupted.
        if(errorScore == 0) { 
            printf("   Line %i Length %i\n", i, len);
            incompleteLinesOnly[incompleteLineCount] = (char*)(input.data + input.cursor);
            incompleteLineCount++;
        } else {
            printf("<< Error on line %i with len %i\n\n", i, len);
        }
        totalErrorScore += errorScore;
        input.cursor = end + 1;
    }

    printf("------- PART 2 -------\n");
    printf("Incomplete lines only: %i\n", incompleteLineCount);
    u64 scoreList[100] = {};
    
    for(u32 i = 0; i < incompleteLineCount; i++) {
        char* line = incompleteLinesOnly[i];
        u32 end = find_next(line, '\n');
        scoreList[i] = autocomplete(line);
    }

    sort_scores(scoreList, incompleteLineCount);
    u64 middleScore = 0;

    for(u32 i = 0; i < incompleteLineCount; i++) {
        printf("%i: %llu ", i+1, scoreList[i]);
        if(i == ((incompleteLineCount-1) / 2)) {
            printf(" !!! middle score\n");
            middleScore = scoreList[i];
        } else {
            printf("\n");
        }
    }

    printf("-----------\n");
    printf("Part 1: Total error score: %i\n", totalErrorScore);
    printf("Part 2: Middle score: %llu\n", middleScore);

    free_file(input);
    return 0;
}