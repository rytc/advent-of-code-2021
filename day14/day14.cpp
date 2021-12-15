#include "../utils/utils.h"

struct Rule {
    char match[2];
    char insert;
};

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");
    u32 lineCount = count_newlines(input);

    char polymerTemplate[20] = {};
    memcpy(polymerTemplate, input.data, 20);
    input.cursor += 21;

    Rule rules[99] = {};
    u32 ruleCount = 0;

    for(u32 i = 2; i < lineCount-1; i++) {
        char line[8] = {};
        memcpy(line, input.data + input.cursor, 8);
        input.cursor += 8;

        Rule* newRule = rules + ruleCount;
        newRule->match[0] = line[0];
        newRule->match[1] = line[1];
        newRule->insert = line[6];

        printf("%s", line);
        printf("%c%c | %c", newRule->match[0], newRule->match[1], newRule->insert);
    }


    free_file(input);
    return 0;
}