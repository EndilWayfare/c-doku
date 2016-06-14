#ifndef SUDOKU_UNDO_H
#define SUDOKU_UNDO_H

#include <stdbool.h>

#include "sudoku_board.h"
#include "sudoku_utility.h"

struct SudokuBoard;

/**
* Struct representing a single change to a sudoku board.
*/
struct HistoryStep {
     Coord2D location; /**< row/column of the square that changed */
     char newValue;    /**< value of the square after change */
     char oldValue;    /**< value of the square before changed */
};

typedef struct HistoryStep HistoryStep;


struct HistoryStruct;

typedef struct HistoryStruct *History;

History createHistory(struct SudokuBoard *owner);

void freeHistory(History *historyPtr);

void addUndoStep(History history, struct Coord2D *square, int value);


bool undoStep(History history, size_t stepsToUndo);

bool redoStep(History history, size_t stepsToRedo);

void invalidateSubsequentRedoSteps(History history);

#endif // !SUDOKU_UNDO_H
