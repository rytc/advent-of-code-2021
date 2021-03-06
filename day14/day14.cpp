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

constexpr u32 HASH_OFFSET = 1000;

static u32
p2_hash(char a, char b) {
    return (u32)a + ((u32)b*HASH_OFFSET);
}

// Part 2 attempt 1 with hashmap craziness
static u64
p2_insertion(u8* srcBuffer, u32 count, Rule* rules, u32 ruleCount) {
    u64 result = 0;
    std::unordered_map<u32, s64> maps[2];
    u32 src = 0;
    u32 dst = 1;
    s64 counts[26] = {};

    // First, build out the initial set of pairs
    for(u32 i = 0; i < count-1; i++) {
        u32 tmpl = p2_hash(srcBuffer[i], srcBuffer[i+1]);
        maps[src][tmpl] = 1;
    }

    for(u32 i = 0; i < count; i++) {
       counts[srcBuffer[i] - ASCII_OFFSET] += 1;
    }

    // This should be 40 steps for part 2,
    // however it is set to 10 for testing to get it to match part 1/p1_insertions
    for(u32 step = 0; step < 10; step++) {
        printf("Step %i\n", step+1);

        #if 0
        for(auto itr = maps[src].begin(); itr != maps[src].end(); itr++) {
            u8 second = static_cast<u8>(itr->first / HASH_OFFSET);
            u8 first = static_cast<u8>(itr->first - (second * HASH_OFFSET));
            printf("%c%c %lli\n", first,second,itr->second);
        }
        #endif
        for(u32 i = 0; i < 26; i++) {
            printf("%c %lli\n", i + ASCII_OFFSET, counts[i]);
        }

        getchar();

        // Loop through each pair in the source map
        for(auto itr = maps[src].begin(); itr != maps[src].end(); itr++) {
            u8 second = static_cast<u8>(itr->first / HASH_OFFSET);
            u8 first = static_cast<u8>(itr->first - (second * HASH_OFFSET));
            if(itr->second <= 0) continue;

            for(u32 ruleIndex = 0; ruleIndex < ruleCount; ruleIndex++) {
                Rule rule = rules[ruleIndex];

                // If the pair matches a rule
                if(rule.match[0] == first && rule.match[1] == second) {

                    // AC -> ABC
                    // new pairs: AB BC
                    u32 newPair = p2_hash(rule.match[0], rule.insert);
                    u32 newPair2 = p2_hash(rule.insert, rule.match[1]);

                    assert(itr->second > 0);

                    // There are cases where a new pair
                    // matches the iterator. For example
                    // PB might have a rule to insert a B
                    // new pairs would be PB BB. 
                    // Since PB matches the iterator, save the count,
                    // reset to 0 since it's a "new" pair, then add the count back
                    // this prevents it from doubling
                    s64 count = itr->second;

                    // Only subtract if the dst already has the pair
                    // and prevent it from going negative
                    if(maps[dst].find(itr->first) != maps[dst].end()) {
                        maps[dst][itr->first] -= MIN(maps[dst][itr->first], count);
                    }

                    // Insert or increment the first pair
                    maps[dst][newPair] += count;
                    // Insert or increment the second pair
                    maps[dst][newPair2] += count;

                    counts[rule.match[0] - ASCII_OFFSET] += count;
                    counts[rule.match[1] - ASCII_OFFSET] += count;
                    counts[rule.insert   - ASCII_OFFSET] += count;

                    break;
                } // Rules match
            } // Rules loop
        } // Pair loop

        // Move what was written into dst
        // into src
        maps[src] = std::move(maps[dst]);

    } // Step loop

    s64 minElements = INT64_MAX;
    s64 maxElements = 0;
    s64 totalElements = 0;

    for(u32 i = 0; i < 26; i++) {
        totalElements += counts[i];
        maxElements = MAX(maxElements, counts[i]);
        if(counts[i] > 0) minElements = MIN(minElements, counts[i]);
    }


    printf("------------\n");

    printf("Part 2 element count: %lli\n", totalElements);
    printf("Part 2 Most common element count: %lli\n", maxElements);
    printf("Part 2 Least common element count: %lli\n", minElements);
    printf("Part 2 result: %llu\n", maxElements - minElements);

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

    printf("Rule count: %i\n", ruleCount);

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


    
    
    free(bufferA); bufferB = nullptr;
    free_file(input);
    return 0;
}