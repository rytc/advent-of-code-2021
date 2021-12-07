#include "../utils/utils.h"

constexpr u32 BOARD_DIM = 1000;

struct Line {
    s32 startX, startY;
    s32 endX, endY;
};

static void
clear_array_char(char *array, u32 count) {
    for(u32 i = 0; i < count; i++) array[i] = 0;
}

static u32
get_pos_index(s32 x, s32 y) {
    return x + (y * BOARD_DIM);
}

static void
mark_line(u16* board, Line line) {
    s32 minX = MIN(line.startX, line.endX);
    s32 maxX = MAX(line.startX, line.endX);
    s32 minY = MIN(line.startY, line.endY);
    s32 maxY = MAX(line.startY, line.endY);

    if(minX == maxX || minY == maxY) {
        for(s32 y = minY; y <= maxY; y++) {
            for(s32 x = minX; x <= maxX; x++) {
                u32 index = get_pos_index(x,y);
                board[index] += 1;
            }
        }
    } else {
        while(minX <= maxX || minY <= maxY) {
            u32 index = get_pos_index(minX, minY);
            board[index] += 1;
            minX += 1;
            minY += 1;
        }
    }
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 lineCount = count_newlines(input);
    printf("Line count: %i\n", lineCount);
    Line* lines = (Line*)malloc(lineCount * sizeof(Line));

    for(u32 i = 0; i < lineCount; i++) {
        Line* line = &lines[i];
        char num[16] = {};

        u32 comma = find_next(input, ',');
        memcpy(num, input.data + input.cursor, comma - input.cursor);
        line->startX = atoi(num);
        input.cursor = comma + 1;
        
        comma = find_next(input, ' ');
        clear_array_char(num, 16);
        memcpy(num, input.data + input.cursor, comma - input.cursor);
        line->startY = atoi(num);
        input.cursor = comma + 3;

        comma = find_next(input, ',');
        clear_array_char(num, 16);
        memcpy(num, input.data + input.cursor, comma - input.cursor);
        line->endX = atoi(num);
        input.cursor = comma + 1;

        comma = find_next(input, '\n');
        clear_array_char(num, 16);
        memcpy(num, input.data + input.cursor, comma - input.cursor);
        line->endY = atoi(num);
        input.cursor = comma + 1;
    
        //printf("(%i, %i) -> (%i, %i)\n", line->startX, line->startY, line->endX, line->endY);
    }

    u16* board = (u16*)calloc((BOARD_DIM*BOARD_DIM), sizeof(u16));

    // Part 1 - only strictly vertical and horizontal lines
    // 8111
    // Part 2 - all lines
    for(u32 i = 0; i < lineCount; i++) {
        mark_line(board, lines[i]);
    }

    u32 intersections = 0;
    for(u32 i = 0; i < (BOARD_DIM*BOARD_DIM); i++) {
        if(board[i] >= 2) intersections += 1;
    }

    printf("Intersections: %i\n", intersections);

    free(board);
    free(lines);
    free_file(input);
    return 0;
}