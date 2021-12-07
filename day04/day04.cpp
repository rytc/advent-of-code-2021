#include "../utils/utils.h"

struct Game_Board_Slot {
    s32 value;
    b32 marked;
};

struct Game_Board {
     Game_Board_Slot slots[25];
     u32 score;
};

static void
mark_board(Game_Board *board, s32 num) {
    for(u32 k = 0; k < 25; k++) {
        if(board->slots[k].value == num) {
            board->slots[k].marked = 1;
        }
    }
}

static void
mark_boards(Game_Board* boards, u32 boardCount, s32 num) {
    for(u32 i = 0; i < boardCount; i++) {
       mark_board(&boards[i], num);
    }
}

static b32 
check_win(Game_Board board) {
    // check rows
    for(u32 r = 0; r < 5; r++) {
        u32 rowIndex = r * 5;
        if(board.slots[r].marked &&
            board.slots[r+1].marked &&
            board.slots[r+2].marked &&
            board.slots[r+3].marked &&
            board.slots[r+4].marked) {
                return true;
        }
    }

    // check columns
    for(u32 c = 0; c < 5; c++) {
        if(board.slots[c].marked &&
            board.slots[c+5].marked &&
            board.slots[c+10].marked &&
            board.slots[c+15].marked &&
            board.slots[c+20].marked) {
                return true;
        }
    }

    return false;
}

static Game_Board*
check_all_for_win(Game_Board* boards, u32 boardCount) {
    for(u32 boardIndex = 0; boardIndex < boardCount; boardIndex++) {
        Game_Board *board = &boards[boardIndex];
        if(check_win(*board)) return board;
    }

    return nullptr;
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 newLineCount = count_newlines(input);
    u32 firstLineLen = find_next(input, '\n');
    s32 *drawableNumbers = (s32*)malloc(firstLineLen * sizeof(s32));
    s32 drawableNumberCount = 0;

    // First read drawable numbers
    for(s32 i = 0; i < firstLineLen; i++) {
        u32 firstComma = find_next(input, ',');

        if(firstComma > firstLineLen) {
            firstComma = firstLineLen;
        }

        char num[8] = {};
        memcpy(num, &input.data[input.cursor], firstComma);
        drawableNumbers[drawableNumberCount] = atoi(num);
        //printf("%i ", drawableNumbers[drawableNumberCount]);
        input.cursor = firstComma + 1;
        drawableNumberCount++;

        if(firstComma == firstLineLen) break;
    }

    printf("Drawable numbers: %i\n", drawableNumberCount);
    input.cursor += 1;

    Game_Board* boards = (Game_Board*)calloc((input.length - input.cursor) / 5, sizeof(Game_Board));
    u32 boardCount = 0;

    // Read boards
    while(input.cursor < input.length) {
        char line[16] = {};
        readline_string_len(&input, line, 16);
        if(line[0] == '\n') {
            continue;
        }
        Game_Board* board = &boards[boardCount];

        for(s32 r = 0; r < 5; r++) {
            s32 rowIndex = r * 5;
            char num[3] = {};
            memcpy(num, line, 3);
            board->slots[rowIndex].value = atoi(num);
            memcpy(num, line + 3, 3);
            board->slots[rowIndex+1].value = atoi(num);
            memcpy(num, line + 6, 3);
            board->slots[rowIndex+2].value = atoi(num);
            memcpy(num, line + 9, 3);
            board->slots[rowIndex+3].value = atoi(num);
            memcpy(num, line + 12, 3);
            board->slots[rowIndex+4].value = atoi(num);

            #if 0
            printf("%i %i %i %i %i\n", board->slots[rowIndex].value,
                    board->slots[rowIndex+1].value,
                    board->slots[rowIndex+2].value,
                    board->slots[rowIndex+3].value,
                    board->slots[rowIndex+4].value);
            #endif
            readline_string_len(&input, line, 16);
        }

        //printf("\n");
        boardCount += 1;
    }

    printf("Total number of boards: %i\n", boardCount);

    for(u32 i = 0; i < drawableNumberCount; i++) {
        mark_boards(boards, boardCount, drawableNumbers[i]);
        // Can only win after 5 numbers
        if(i >= 5) {
            Game_Board *winner = check_all_for_win(boards, boardCount);
            if(winner) {
                u32 sum = 0;
                for(u32 slotIndex = 0; slotIndex < 25; slotIndex++) {
                    Game_Board_Slot slot = winner->slots[slotIndex];
                    if(!slot.marked) {
                        sum += slot.value;
                    }
                }
                winner->score = sum * drawableNumbers[i];
                printf("First winner: %i\n", winner->score);
                break;
            }
        }
    }

    
    u32 lastScore = 0;
    Game_Board *lastToWin = nullptr;
    for(u32 i = 0; i < drawableNumberCount; i++) {
        u32 winCount = 0;
        mark_boards(boards, boardCount, drawableNumbers[i]);

        for(u32 boardIndex = 0; boardIndex < boardCount; boardIndex++) {
            Game_Board *board = &boards[boardIndex];
            b32 won = check_win(*board);
            if(won) {
                winCount += 1;
            }

            if(winCount == boardCount-2) {
                for(u32 k = 0; k < boardCount; k++) {
                    if(!check_win(boards[k])) {
                        lastToWin = &boards[k];
                        printf("Last board to win: %i\n", k);
                        goto finish;
                    }
                }
            }
        }
    }

    finish:
    if(lastToWin) {
        for(u32 i = 0; i < drawableNumberCount; i++) {
            mark_board(lastToWin, drawableNumbers[i]);
            if(check_win(*lastToWin)) {
                printf("Last board won with %i\n", drawableNumbers[i]);

                for(u32 c = 0; c < 25; c++) {
                    if(lastToWin->slots[c].marked) { 
                        printf("x  ");
                    } else {
                        printf("%i ", lastToWin->slots[c].value);
                    }

                    if(c == 4 || c == 9 || c == 14 || c == 19) printf("\n");
                }

                u32 sum = 0;
                for(u32 slotIndex = 0; slotIndex < 25; slotIndex++) {
                    Game_Board_Slot slot = lastToWin->slots[slotIndex];
                    if(!slot.marked) {
                        sum += slot.value;
                    }
                }
                lastToWin->score = sum * drawableNumbers[i];
                printf("Score of last to win: %i\n", lastToWin->score);
                break;
            }
        }    
    } else {
        printf("No last winner?!?\n");
    }

    free(boards);
    free(drawableNumbers);
    free_file(input);
    return 0;
}