#include "../utils/utils.h"
#include <cassert>

struct Rule {
    char match[2];
    char insert;
};

#define KB(v) (v*1024L)
#define MB(v) (KB(v)*1024L)

constexpr u64 BUFFER_SIZE = MB(1024);

// I think this is UB or source of slowness
// compiler/cpu needs checks to makes sure srcbuffer and dstBuffer are not at the same location
//
static u32
p1_insertion(u8* srcBuffer, u8* dstBuffer, u32 srcCount, Rule* rules, u32 ruleCount) {
    u32 writeCursor = 0;
    for(u32 readCursor = 0; readCursor < srcCount;) {
        assert(srcBuffer[readCursor + 0] != '\n' && srcBuffer[readCursor + 1] != '\n');
        assert(readCursor < BUFFER_SIZE);
        assert(writeCursor < BUFFER_SIZE);

        b32 ruleMatch = false; 
        for(u32 i = 0; i < ruleCount; i++) {
            Rule rule = rules[i];
            if(srcBuffer[readCursor + 0] == rule.match[0] && srcBuffer[readCursor + 1] == rule.match[1]) {

                dstBuffer[writeCursor + 0] = rule.match[0];
                dstBuffer[writeCursor + 1] = rule.insert;
                dstBuffer[writeCursor + 2] = rule.match[1];

                readCursor += 1;
                writeCursor += 2;
                ruleMatch = true;
                break;
            }
        }

        if(!ruleMatch) {
            // if there is no match, then still write it out to the dst
            dstBuffer[writeCursor + 0] = srcBuffer[readCursor + 0];
            dstBuffer[writeCursor + 1] = srcBuffer[readCursor + 1];
            writeCursor += 1;
            readCursor += 1;
        }
    }
    dstBuffer[writeCursor] = '\0';

    assert(writeCursor > 0);
    return writeCursor;
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

    // Part 1
    u32 p1MaxElement = 0;
    u32 p1MinElement = 99999;
    u32 p1SrcCount = 20;
    {
        u8* src = bufferA;
        u8* dst = bufferB;

        printf("Template: %s\n", src);
        for(u32 i = 0; i < 10; i++) {
            p1SrcCount = p1_insertion(src, dst, p1SrcCount, rules, ruleCount);
            printf("Step %i\n", i+1, dst);
            u8* swap = src;
            src = dst;
            dst = swap;
        }

        u32 elementCounts[26] = {};

        // Count the elements
        for(u32 i = 0; i < p1SrcCount; i++) {
            // src is the last buffer that was written to
            elementCounts[src[i] - 65] += 1;
        }

        for(u32 e = 0; e < 26; e++) {
            p1MaxElement = MAX(p1MaxElement, elementCounts[e]);
            // not every alphabet character is used
            if(elementCounts[e] > 0) p1MinElement = MIN(p1MinElement, elementCounts[e]);
        }
    }

    // Part 2
    #if 0 // Will come back to this. 40 steps is too many to brute force
    u64 p2MaxElement = 0;
    u64 p2MinElement = 99999;
    u64 p2SrcCount = 20;
    {
        memcpy(bufferA, input.data, 20);
        bufferA[20] = '\0';
        u8* src = bufferA;
        u8* dst = bufferB;
        u64 srcCount = 20;

        printf("Template: %s\n", src);
        for(u32 i = 0; i < 40; i++) {
            p2SrcCount = p1_insertion(src, dst, p2SrcCount, rules, ruleCount);
            printf("Step %i\n", i+1, dst);
            u8* swap = src;
            src = dst;
            dst = swap;
        }

        u64 elementCounts[26] = {};

        // Count the elements
        for(u64 i = 0; i < p2SrcCount; i++) {
            // src is the last buffer that was written to
            elementCounts[src[i] - 65] += 1;
        }

        for(u32 e = 0; e < 26; e++) {
            p2MaxElement = MAX(p2MaxElement, elementCounts[e]);
            // not every alphabet character is used
            if(elementCounts[e] > 0) p2MinElement = MIN(p2MinElement, elementCounts[e]);
        }
    }
    #endif 

    printf("------------\n");
    printf("Part 1 count: %i\n", p1SrcCount);
    printf("Part 1 Most common element count: %i\n", p1MaxElement);
    printf("Part 1 Least common element count: %i\n", p1MinElement);
    printf("Part 1 result: %i\n", p1MaxElement - p1MinElement);

    #if 0
    printf("Part 2 count: %llu\n", p2SrcCount);
    printf("Part 2 Most common element count: %llu\n", p2MaxElement);
    printf("Part 2 Least common element count: %llu\n", p2MinElement);
    printf("Part 2 result: %llu\n", p2MaxElement - p2MinElement);
    #endif

    
    free(bufferA); bufferB = nullptr;
    free_file(input);
    return 0;
}