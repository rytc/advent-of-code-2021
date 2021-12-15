#include "../utils/utils.h"
#include <cassert>
#include <unordered_map>


struct Rule {
    char match[2];
    char insert;
};

#define KB(v) (v*1024L)
#define MB(v) (KB(v)*1024L)

constexpr u64 BUFFER_SIZE = MB(1024);
constexpr u32 ASCII_OFFSET = 65; 

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

constexpr u32 HASH_OFFSET = 10000;

static u32
p2_hash(char a, char b) {
    return (u32)a + ((u32)b*HASH_OFFSET);
}

static u64
p2_insertion(u8* srcBuffer, u32 count, Rule* rules, u32 ruleCount) {
    u64 result = 0;
    std::unordered_map<u32, s64> maps[2];

    // First, build out the initial set of pairs
    for(u32 i = 0; i < count-1; i++) {
        u32 tmpl = p2_hash(srcBuffer[i], srcBuffer[i+1]);
        maps[0][tmpl] += 1;
    }

    // Copy the first set of pairs into the second map
    maps[1] = maps[0];

    u32 src = 0;
    u32 dst = 1;
    // This should be 40 steps for part 2,
    // however it is set to 10 for testing to get it to match part 1/p1_insertions
    for(u32 step = 0; step < 10; step++) {
        printf("Step %i\n", step+1);

        // Loop through each pair in the source map
        for(auto itr = maps[src].begin(); itr != maps[src].end(); itr++) {
            u8 second = (u8)(itr->first / HASH_OFFSET);
            u8 first = (u8)(itr->first - (second * HASH_OFFSET));

            for(u32 ruleIndex = 0; ruleIndex < ruleCount; ruleIndex++) {
                Rule rule = rules[ruleIndex];

                // If the pair matches a rule
                if(rule.match[0] == first && rule.match[1] == second) {
                    // AC -> ABC
                    // new pairs: AB BC
                    u32 newPair = p2_hash(rule.match[0], rule.insert);
                    u32 newPair2 = p2_hash(rule.insert, rule.match[1]);
                    
                    // Insert or increment the first pair
                    maps[dst][newPair] += itr->second;

                    // Insert or increment the second pair
                    maps[dst][newPair2] += itr->second;
                    
                    // Decrement the pair since we are inserting
                    // a character in the middle, breaking the pair

                    maps[dst][p2_hash(first, second)] -= itr->second;
                    itr->second = 0;
                    
                    // If we matched a rule, then no other rules should match 
                    break;
                }
            }
        }

        // Copy what was written into dst
        // into src
        maps[src] = maps[dst];

        // Flip the indices so we write into "src" and read from "dst"
        // on the next iteration. This is because we shouldn't iterate over
        // a container that we are adding to.
        u32 tmp = src;
        src = dst;
        dst = tmp;
    }

    printf("Part 2 pair count: %llu\n", maps[src].size());
    u64 minElements = UINT64_MAX;
    u64 maxElements = 0;

    s64 counts[26] = {};
    s64 totalElements = 0;
    for(auto itr = maps[src].begin(); itr != maps[src].end(); itr++) {
        u8 second = itr->first / HASH_OFFSET;
        u8 first = itr->first - (second * HASH_OFFSET);

        //printf("%c%c %lli\n", first,second,itr->second);
        totalElements += itr->second * 2;
        counts[first - ASCII_OFFSET] += itr->second;
        counts[second - ASCII_OFFSET] += itr->second;
    }

    for(u32 i = 0; i < 26; i++) {
        maxElements = MAX(maxElements, counts[i]);
        if(counts[i] > 0) minElements = MIN(minElements, counts[i]);
    }

    printf("Part 2 element count: %lli\n", totalElements);
    printf("Part 2 Most common element count: %llu\n", maxElements);
    printf("Part 2 Least common element count: %llu\n", minElements);

    return maxElements - minElements;
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
            elementCounts[src[i] - ASCII_OFFSET] += 1;
        }

        for(u32 e = 0; e < 26; e++) {
            p1MaxElement = MAX(p1MaxElement, elementCounts[e]);
            // not every alphabet character is used
            if(elementCounts[e] > 0) p1MinElement = MIN(p1MinElement, elementCounts[e]);
        }
    }

    // Part 2
    u64 p2Difference = 0;
    {
        memcpy(bufferA, input.data, 20);
        p2Difference = p2_insertion(bufferA, 20, rules, ruleCount);
    }

    printf("------------\n");
    printf("Part 1 count: %i\n", p1SrcCount);
    printf("Part 1 Most common element count: %i\n", p1MaxElement);
    printf("Part 1 Least common element count: %i\n", p1MinElement);
    printf("Part 1 result: %i\n", p1MaxElement - p1MinElement);
    printf("Part 2 result: %llu\n", p2Difference);


    
    free(bufferA); bufferB = nullptr;
    free_file(input);
    return 0;
}