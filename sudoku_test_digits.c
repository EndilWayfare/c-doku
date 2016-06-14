/******************************************************************************
 * Program: sudoku_test_digits.c
 *
 * Purpose: Functions for evaluating the sudoku board solution: which digits
 *          are present and if solution is valid
 *
 * Developer: Philip Ormand
 *
 * Date: 5/13/16
 *
 *****************************************************************************/
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "sudoku_board.h"
#include "sudoku_test_digits.h"

/**
 * Required for the functionality of the generic 'EVALUATE_DIGIT' macro
 */
enum SUDOKU_COLLECTION_TYPE {
     // strange capitalization = parallelism with members of DigitsPresent struct
     SUDOKU_IS_rows,
     SUDOKU_IS_columns,
     SUDOKU_IS_blocks,
};


void reportIllegalDigit(int row, int col, int value);
void reportRepeatedDigit(enum SUDOKU_COLLECTION_TYPE type, int row, int col, int block, int value);
void reportBlankSquare(int row, int col);

/**
 * Business logic of a single iteration, checking if a single digit is present in a single row,
 * column, or block.
 */
#define EVALUATE_DIGIT(type, collectionNumber)                                                \
if (validateSudokuDigit(value))                                                               \
{                                                                                             \
     /* if square is blank */                                                                 \
     if (value == 0)                                                                          \
     {                                                                                        \
          /* if caller wants printed feedback AND we haven't reported on this square yet */   \
          if (verbose && !digitsPresent->squaresIllegalOrBlank[i][j])                          \
          {                                                                                   \
               reportBlankSquare(i, j);                                                       \
          }                                                                                   \
                                                                                              \
          digitsPresent->squaresIllegalOrBlank[i][j] = true;                                   \
          sudokuValid = false;                                                                \
     }                                                                                        \
     /* square has a valid sudoku digit */                                                    \
     else                                                                                     \
     {                                                                                        \
          currentTestFlag = SUDOKU_TEST_FLAG_SHIFT(value);                                    \
                                                                                              \
          /* if SUDOKU_TEST_<caseNumber> flag is set */                                       \
          if (digitsPresent->type[collectionNumber] & currentTestFlag)                         \
          {                                                                                   \
               /* this digit is repeated, so solution is invalid */                           \
               sudokuValid = false;                                                           \
                                                                                              \
               /* print feedback message if caller wants it */                                \
               if (verbose) {                                                                 \
                    reportRepeatedDigit(SUDOKU_IS_##type, i, j, k, value);                    \
               }                                                                              \
          }                                                                                   \
          /* we haven't encountered this digit yet in the current row */                      \
          else                                                                                \
          {                                                                                   \
               /* mark the digit as present */                                                \
               digitsPresent->type[collectionNumber] |= currentTestFlag;                       \
          }                                                                                   \
     }                                                                                        \
}                                                                                             \
/* illegal sudoku digit */                                                                    \
else                                                                                          \
{                                                                                             \
     /* if caller wants printed feedback AND we haven't reported on this square yet */        \
     if (verbose && !digitsPresent->squaresIllegalOrBlank[i][j])                               \
     {                                                                                        \
          reportIllegalDigit(i, j, value);                                                    \
     }                                                                                        \
                                                                                              \
     digitsPresent->squaresIllegalOrBlank[i][j] = true;                                        \
     sudokuValid = false;                                                                     \
}

/** 
 * Macro to retreive the value of the current sudoku square
 */
#define CURRENT_ITEM board->contents[i][j]

/**
 * Populates a DigitsPresent struct with the correct values for the current state of a sudoku board.
 * Analyzes sudoku board contents to determine which digits are present in each row, column, and
 * block, whether any squares are blank or have illegal values, and whether any digits were repeated
 * within a row, column, or block (invalidating the sudoku solution).
 *
 * @param board Pointer to the SudokuBoard to analyze
 * @param digitsPresent Pointer to the DigitsPresent struct that will store the results
 * @param verbose If true, results of analysis will be printed to the screen.
 */
void evaluateDigitsPresent(struct SudokuBoard *board, struct DigitsPresent *digitsPresent, bool verbose)
{
     int i, j, k = 0, value;
     bool sudokuValid = true;
     SudokuDigitTestField currentTestFlag = 0;

     // reset DigitsPresent member
     memset((void*)digitsPresent, 0, sizeof(DigitsPresent));

     // rows
     for (i = 0; i < SUDOKU_ROW_COUNT; ++i)
     {
          for (j = 0; j < SUDOKU_COL_COUNT; ++j)
          {
               value = CURRENT_ITEM;
               
               EVALUATE_DIGIT(rows, i)
          }

          // at this point, the whole row should be evaluated
          if (verbose && digitsPresent->rows[i] != SUDOKU_TEST_ALLDIGITS)
          {
               // condition is 0 (if-statement fails) if all the same bits are set/unset in current row
               // and in SUDOKU_TEST_ALLDIGITS
               // contition is non-0 (if-statement succeeds) if there is a bit set in SUDOKU_TEST_ALLDIGITS
               // that is not set in current row, meaning there is a digit NOT present in the row
               printf("OOPS: row %d doesn't contain all digits from 1 to 9\n", i + 1);
          }
     }

     // columns
     for (j = 0; j < SUDOKU_COL_COUNT; ++j)
     {
          for (i = 0; i < SUDOKU_ROW_COUNT; ++i)
          {
               value = CURRENT_ITEM;

               EVALUATE_DIGIT(columns, j)
          }

          // at this point, the whole column should be evaluated
          if (verbose && digitsPresent->columns[j] != SUDOKU_TEST_ALLDIGITS)
          {
               printf("OOPS: column %c doesn't contain all digits from 1 to 9\n", colLabels[j]);
          }
     }

     // Blocks
     for (k = 0; k < SUDOKU_BLOCK_COUNT; ++k)
     {
          for (i = (k / SUDOKU_BLOCK_HEIGHT) * SUDOKU_BLOCK_HEIGHT; 
               i < (k / SUDOKU_BLOCK_HEIGHT) * SUDOKU_BLOCK_HEIGHT + SUDOKU_BLOCK_HEIGHT;
               ++i)
          {
               for (j = (k % SUDOKU_BLOCK_WIDTH) * SUDOKU_BLOCK_WIDTH;
               j < (k % SUDOKU_BLOCK_WIDTH) * SUDOKU_BLOCK_WIDTH + SUDOKU_BLOCK_WIDTH;
                    ++j)
               {
                    value = CURRENT_ITEM;

                    EVALUATE_DIGIT(blocks, k)
               }
          }

          // at this point, the whole block should be evaluated
          if (verbose && digitsPresent->blocks[k] != SUDOKU_TEST_ALLDIGITS)
          {
               printf("OOPS: block %d doesn't contain all digits from 1 to 9\n", k + 1);
          }
     }

     if (verbose && sudokuValid)
     {
          printf("Sudoku solution is valid!\n");
     }
}

/**
 * Prints feedback to STDOUT, reporting that a square contains an illegal value
 * Inner function of 'evaluateDigitsPresent'
 *
 * @param row Index of square's row
 * @param col Index of square's column
 * @param value Square's current value, which is illegal
 */
inline void reportIllegalDigit(int row, int col, int value)
{
     printf("OOPS: the value '%d' is not a legal sudoku number (%c%d)\n", value, colLabels[col], row + 1);
}

/**
* Prints feedback to STDOUT, reporting that a square contains a digit repeated elsewhere in its row,
* colulmn, or block.
* Inner function of 'evaluateDigitsPresent'
*
* @param type Reporting problem with a row, column, or block?
* @param row Index of square's row
* @param col Index of square's column
* @param block Index of square's block
* @param value Square's current value, which is illegal
*/
inline void reportRepeatedDigit(enum SUDOKU_COLLECTION_TYPE type, int row, int col, int block, int value)
{
     printf("OOPS: the digit '%d' was repeated in ", value);
     
     // different message, depending on if we're testing a row, column, or block
     switch (type)
     {
     case SUDOKU_IS_rows:
          printf("row %d ", row + 1);
          break;

     case SUDOKU_IS_columns:
          printf("column %c ", colLabels[col]);
          break;

     case SUDOKU_IS_blocks:
          printf("block %d ", block + 1);
          break;

     default:
          // there should never be a default case, because 'type' is an enum value
          puts("...just kidding. Something went terribly wrong.");
          break;
     }
          
     // print coordinates of the square in question
     printf("(%c%d)\n", colLabels[col], row + 1);
}

/**
* Prints feedback to STDOUT, reporting that a square is blank.
* Inner function of 'evaluateDigitsPresent'
*
* @param row Index of square's row
* @param col Index of square's column
*/
inline void reportBlankSquare(int row, int col)
{
     printf("OOPS: square %c%d is blank\n", colLabels[col], row + 1);
}