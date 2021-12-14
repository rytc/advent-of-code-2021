#include "../utils/utils.h"
#include <cassert>

u32 testWidth = 10;
u32 testHeight = 7; 
u8 testGrid[] = {
    1, 0, 1, 1, 0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 0, 1, 0, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static void
print_test_grid() {
    for(u32 y = 0; y < testHeight; y++) {
        for(u32 x = 0; x < testWidth; x++) {
            printf("%i ", testGrid[x + (y*testWidth)]);
        }
        printf("\n");
    }
}

struct Coord {
    u16 x;
    u16 y;
};

struct Fold_Instruction {
    char axis;
    u16 offset;
};

static void 
print_grid(u8* grid, u32 width, u32 height) {
    for(u32 y = 0; y < height; y++) {
        for(u32 x = 0; x < width; x++) {
            printf("%i ", grid[x + (y*width)]);
        }
    }
}

u32 bleh = 0;
u32 bleh2 = 0;
static void 
fold(u8* grid, u32 width, u32 height, Fold_Instruction fold) {
    u32 overlapCount = 0;
    if(fold.axis == 'y') {
        for(u32 y = 0; y < height-fold.offset; y++) {
            for(u32 x = 0; x < width; x++) {
                u32 tmpY = y + fold.offset;
                u32 srcIndex = x + (tmpY*width);
                tmpY = fold.offset - (y+1);
                u32 dstIndex = x + (tmpY * width);
                if(grid[srcIndex] > 0) grid[dstIndex] = grid[srcIndex];
            }
        }
    } else if(fold.axis == 'x') {
        for(u32 y = 0; y < height; y++) {
            for(u32 x = 0; x < fold.offset; x++) {
                s32 tmpX = x + fold.offset;
                u32 srcIndex = tmpX + (y * width);
                tmpX = fold.offset - (x+1);
                if(tmpX >= 0) {
                    u32 dstIndex = tmpX + (y * width);
                    if(grid[srcIndex] > 0 && grid[dstIndex] > 0) {
                        overlapCount++;
                    }
                    grid[dstIndex] += grid[srcIndex];
                    grid[srcIndex] = 0;
                }
            }
        }
    }
    printf("overlaps: %i\n", overlapCount);
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    // TEST
    {
        printf("---- Test data: fold along x = 5\n");
        print_test_grid();
        fold(testGrid, testWidth, testHeight, Fold_Instruction{'x', 5});
        print_test_grid();
        printf("-------\n");
    }


    u32 maxX = 0;
    u32 maxY = 0;

    Coord* coords = (Coord*)malloc(880 * sizeof(Coord));
    u32 coordCount = 0;

    Fold_Instruction foldInstructions[12] = {};
    u32 foldInstCount = 0;

    u32 lineCount = count_newlines(input);

    // Parse input
    for(u32 i = 0; i < lineCount; i++) {
        if(input.data[input.cursor] == '\n') input.cursor++;
        char line[18] = {};
        u32 endl = find_next(input, '\n');
        memcpy(line, input.data + input.cursor, (endl - input.cursor) + 1); // +1 to copy newline

        if(line[0] == 'f') {
            // Fold instruction
            char axis = ' ';
            u32 start = 0; 
            u32 end = 0;

            for(u32 k = 0; k < 18; k++) {
                if(line[k] == 'x' || line[k] == 'y') {
                    axis = line[k];
                }
                if(line[k] == '=') start = k;
                if(line[k] == '\n') end = k;
            }
            
            char num[4] = {};
            memcpy(num, line + start + 1, end - start);

            foldInstructions[foldInstCount] = {axis, (u16)atoi(num)};
            foldInstCount++;

        } else if(coordCount < 879){
            // Coordinates
            char xStr[5] = {};
            char yStr[5] = {};
            xStr[4] = '\0';
            yStr[4] = '\0';

            u32 comma = find_next(line, ',');
            memcpy(xStr, line, comma);

            u32 endl2 = find_next(line, '\n');
            memcpy(yStr, line + comma + 1, endl2 - comma);

            Coord newCoord = Coord{(u16)atoi(xStr), (u16)atoi(yStr)};

            coords[coordCount] = newCoord;
            coordCount++;
            maxX = MAX(maxX, newCoord.x);
            maxY = MAX(maxY, newCoord.y);
            
            //printf("%i,%i\n", newCoord.x, newCoord.y);

        }
        input.cursor = endl+1;
    }

    maxX+=1;
    maxY+=1;
    printf("Max X: %i Max Y: %i\n", maxX, maxY);
    printf("There are %i coordinates\n", coordCount);

    u8* grid = (u8*)calloc(maxX*maxY, sizeof(u8));

    // Fill out the grid
    for(u32 i = 0; i < coordCount; i++) {
        Coord coord = coords[i];
        u32 index = coord.x + (coord.y * maxX);
        grid[index] = 1;
        
    }

    u32 initialDotCount = 0;
    for(u32 y = 0; y < maxY; y++) {
        for(u32 x = 0; x < maxX; x++) {
            if(grid[x + (y*maxX)] > 0) initialDotCount++;
        }
    }

    u32 p1Dots = 0;
    {
        u8* tmpGrid = (u8*)calloc(maxX*maxY, sizeof(u8));
        memcpy(tmpGrid, grid, maxX*maxY*sizeof(u8));
        u32 y = 92;
        for(u32 i = 0; i < maxX; i++) {
            if(i == foldInstructions[0].offset) printf("\n\n");
            if(tmpGrid[i + (y*maxX)] == 0) {
                printf(".", tmpGrid[i + (y*maxX)]);
            } else {
                printf("#");
            }
        }printf("\n");
        
        fold(tmpGrid, maxX, maxY, foldInstructions[0]);

        for(u32 i = 0; i < maxX; i++) {
            if(i == foldInstructions[0].offset) printf("\n\n");
            if(tmpGrid[i + (y*maxX)] == 0) {
                printf(".", tmpGrid[i + (y*maxX)]);
            } else {
                printf("#");
            }
        }printf("\n");


        for(u32 y = 0; y < maxY; y++) {
            for(u32 x = 0; x < foldInstructions[0].offset; x++) {
                if(tmpGrid[x + (y*maxX)] > 0) p1Dots++;
            }
        }
        free(tmpGrid);
    }
    printf("Initial dot count: %i\n", initialDotCount);
    printf("Part 1 dot count: %i\n", p1Dots);

    free(grid);
    free(coords);
    free_file(input);
    return 0;
}