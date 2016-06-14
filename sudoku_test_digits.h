#ifndef SUDOKU_TEST_DIGITS_H
#define SUDOKU_TEST_DIGITS_H

#include <stdbool.h>

#include "sudoku_board.h"
#include "sudoku_utility.h"

struct SudokuBoard;

/**
 * Bit-flags to indicate if a sudoku row, column, or block contains a particular digit
 */
typedef enum {
     SUDOKU_TEST_1 = 1,           // 00000000 00000001
     SUDOKU_TEST_2 = 2,           // 00000000 00000010
     SUDOKU_TEST_3 = 4,           // 00000000 00000100
     SUDOKU_TEST_4 = 8,           // 00000000 00001000
     SUDOKU_TEST_5 = 16,          // 00000000 00010000
     SUDOKU_TEST_6 = 32,          // 00000000 00100000
     SUDOKU_TEST_7 = 64,          // 00000000 01000000
     SUDOKU_TEST_8 = 128,         // 00000000 10000000
     SUDOKU_TEST_9 = 256,         // 00000001 00000000
     SUDOKU_TEST_ALLDIGITS = 511, // 00000001 11111111
} DigitTestFlags;

#define SUDOKU_TEST_FLAG_SHIFT(number) (1 << ((number) - 1))

/**
 * Bit-field for the digits contained in an inidividual row, column, or block.
 * Value reflects the bit-wise combination of various flags from the DigitTestFlag enum
 */
typedef int SudokuDigitTestField;

/**
 * All the bit-fields neccessary to determine if each row, column, and block in a particular
 * sudoku board is valid
 */
struct DigitsPresent {
     SudokuDigitTestField columns[9];   /**< records the digits present in each column */
     SudokuDigitTestField rows[9];      /**< records the digits present in each row */
     SudokuDigitTestField blocks[9];    /**< records the digits present in each block */
     bool squaresIllegalOrBlank[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT]; 
     /**< records whether a square has been reported to have illegal or blank contents */
};

typedef struct DigitsPresent DigitsPresent;

void evaluateDigitsPresent(struct SudokuBoard *board, struct DigitsPresent *digitsPresent, bool verbose);

#endif // !SUDOKU_TEST_DIGITS_H

