#include "../utils/utils.h"
#include <cassert>

struct Rule {
    char match[2];
    char insert;
};

#define KB(v) v*1024L
#define MB(v) KB(v)*1024L

constexpr u64 BUFFER_SIZE = MB(32);

static u32
p1_insertion(u8* srcBuffer, u8* dstBuffer, u32 srcCount, Rule* rules, u32 ruleCount) {
    u32 writeCursor = 0;
    for(u32 readCursor = 0; readCursor < srcCount;) {
        assert(writeCursor < BUFFER_SIZE);

        for(u32 i = 0; i < ruleCount; i++) {
            Rule rule = rules[i];
            if(srcBuffer[readCursor + 0] == rule.match[0] && srcBuffer[readCursor + 1] == rule.match[1]) {

                dstBuffer[writeCursor + 0] = rule.match[0];
                dstBuffer[writeCursor + 1] = rule.insert;
                dstBuffer[writeCursor + 2] = rule.match[1];

                readCursor += 1;
                writeCursor += 2;
                continue;
            }
        }

        // The continue above prevents this write from happening if there is a match
        // if there is no match, then still write it out to the dst
        dstBuffer[writeCursor + 0] = srcBuffer[readCursor + 0];
        dstBuffer[writeCursor + 1] = srcBuffer[readCursor + 1];
        writeCursor += 1;
        readCursor += 1;
    }
    dstBuffer[writeCursor] = '\0';

    if(writeCursor > 0)
        return writeCursor;
    else {
        return 0;
    }
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");
    u32 lineCount = count_newlines(input);

    u8* bufferA = (u8*)calloc(BUFFER_SIZE*2, sizeof(u8));
    u8* bufferB = bufferA + BUFFER_SIZE;

    memcpy(bufferA, input.data, 20);
    input.cursor += 22;

    Rule rules[99] = {};
    u32 ruleCount = 0;

    for(u32 i = 2; i < lineCount; i++) {
        char line[9] = {};
        memcpy(line, input.data + input.cursor, 8);
        input.cursor += 8;

        Rule* newRule = rules + ruleCount;
        newRule->match[0] = line[0];
        newRule->match[1] = line[1];
        newRule->insert = line[6];
        ruleCount++;
        //printf("%c%c | %c\n", newRule->match[0], newRule->match[1], newRule->insert);
    }

    printf("Step 0: %s (Initial template)\n", bufferA);
    u8* src = bufferA;
    u8* dst = bufferB;
    u32 srcCount = 20;
    for(u32 i = 0; i < 10; i++) {
        srcCount = p1_insertion(src, dst, srcCount, rules, ruleCount);
        printf("Step %i: %s\n", i+1, dst);
        u8* swap = src;
        src = dst;
        dst = swap;
    }

    u32 elementCounts[26] = {};

    // Count the elements
    for(u32 i = 0; i < srcCount; i++) {
        // src is the last buffer that was written to
        elementCounts[src[i] - 65] += 1;
    }

    u32 maxElement = 0;
    u32 minElement = 99999;
    for(u32 e = 0; e < 26; e++) {
        maxElement = MAX(maxElement, elementCounts[e]);
        // not every alphabet character is used
        if(elementCounts[e] > 0) minElement = MIN(minElement, elementCounts[e]);
    }

    printf("------------\n");
    printf("Element count: %i\n", srcCount);
    printf("Most common element count: %i\n", maxElement);
    printf("Least common element count: %i\n", minElement);
    printf("Part 1 result: %i\n", maxElement - minElement);
    
    free(bufferA); bufferB = nullptr;
    free_file(input);
    return 0;
}