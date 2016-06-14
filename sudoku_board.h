#ifndef SUDOKU_BOARD_H
#define SUDOKU_BOARD_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sudoku_undo.h"
#include "sudoku_utility.h"

struct SudokuBoard {
     char contents[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT];    /**< NOT a null-terminated string; a collection of small integers */
     History history;
};

typedef struct SudokuBoard SudokuBoard;

//"initialize", not "create", since it is not allocated
void initializeSudokuBoard(struct SudokuBoard *board);

void freeSudokuBoardResources(struct SudokuBoard *board);

bool loadSudokuBoard(char *fileName, struct SudokuBoard *board);

void copySudokuBoardContents(const char source[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT], char destination[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT]);

void printSudokuBoard(struct SudokuBoard *board);

#endif // !SUDOKU_BOARD_H