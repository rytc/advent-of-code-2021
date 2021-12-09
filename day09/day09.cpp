#include "../utils/utils.h"

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 rowCount = count_newlines(input);
    u32 colCount = find_next(input, '\n');
    u8* heightmap = (u8*)malloc((rowCount*colCount)*sizeof(u8));

    for(u32 i = 0; i < input.length; i++) {
        if(input.data[i] == '\n') continue;
        heightmap[i] = input.data[i];
    }

    printf("Last number is %i\n", heightmap[(rowCount*colCount) - 1]);


    free_file(input);
    return 0;
}