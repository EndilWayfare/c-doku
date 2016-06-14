/******************************************************************************
 * Program: sudoku_assistant.c
 *
 * Purpose: Defines the assistant functions that help solve sudoku puzzles
 *
 * Developer: Philip Ormand
 *
 * Date: 5/13/16
 *
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "sudoku_assistant.h"
#include "sudoku_board.h"
#include "sudoku_test_digits.h"


HistoryStep assistantCrosshatch(SudokuBoard *board, bool verbose);
HistoryStep assistantLocked(SudokuBoard *board, bool verbose);
char scanForSingleCandidate(SudokuDigitTestField digitsPossible[][SUDOKU_COL_COUNT], Coord2D *suggestedSquare);


const SudokuAssistant assistants[] = {
     {"crosshatch", "Uses cross - hatch scanning to identify 'hidden singles'", assistantCrosshatch},
     {"locked", "Uses row/column range checking to identify 'locked' candidates", assistantLocked}
};

#define SUDOKU_ASSISTANT_COUNT sizeof(assistants)/sizeof(*assistants)

const char *sudokuAssistantNoSuggestionMessage = "Sorry, no recommendations found using this assistant\n";

HistoryStep assistantCrosshatch(SudokuBoard * board, bool verbose)
{
     HistoryStep suggestion = { 0 };
     DigitsPresent digitsPresent;
     SudokuDigitTestField testFlag;
     int row, column, block, digit;

     // make sure we know which digits are present in each row, column, and block
     evaluateDigitsPresent(board, &digitsPresent, false);

     // NOTE: 'suggestion.newValue' is used to represent whether or not the assistant has found a
     //       value to suggest. If 0 (i.e. false), the loops should continue running. Otherwise,
     //       suggestion has been found and we can break out.
     
     for (row = 0; !suggestion.newValue && row < SUDOKU_ROW_COUNT; ++row)
     {
          // check if each digit is in the row in question
          for (digit = 1; !suggestion.newValue && digit <= SUDOKU_DIGIT_MAX; ++digit)
          {
               testFlag = SUDOKU_TEST_FLAG_SHIFT(digit);

               // if row contains that digit...
               if (digitsPresent.rows[row] & testFlag)
               {
                    // ...check which columns also have it
                    for (column = 0; !suggestion.newValue && column < SUDOKU_COL_COUNT; ++column)
                    {
                         if (digitsPresent.columns[column] & testFlag)
                         {
                              // determine which block the intersection occurs in
                              block = SUDOKU_BLOCK_FROM_INTERSECTION(row, column);

                              // if block doesn't already contain that digit...
                              if (!(digitsPresent.blocks[block] & testFlag))
                              {
                                   // array of Coord2D structs to store blank squares in block
                                   // only 4 possible squares per block, due to row/col intersection
                                   Coord2D candidates[4] = { 0 };

                                   int blockRow, blockColumn, blockRowOffset, blockColumnOffset,
                                        candidateCount = 0;

                                   // iterate through block squares
                                   blockRowOffset = (block / 3) * 3;

                                   for (blockRow = blockRowOffset;
                                        blockRow < SUDOKU_BLOCK_HEIGHT + blockRowOffset; ++blockRow)
                                   {
                                        blockColumnOffset = (block % 3) * 3;
                                        
                                        for (blockColumn = blockColumnOffset;
                                             blockColumn < SUDOKU_BLOCK_WIDTH + blockColumnOffset; ++blockColumn)
                                        {                                             
                                             if ( // if square's value is 0,
                                                  board->contents[blockRow][blockColumn] == 0 && 
                                                  // AND if square is not inside outer row/column,
                                                  blockRow != row && blockColumn != column &&
                                                  // AND square's row doesn't already contain digit,
                                                  !(digitsPresent.rows[blockRow] & testFlag) &&
                                                  // AND square's column doesnt already contain digit...
                                                  !(digitsPresent.columns[blockColumn] & testFlag))
                                             {
                                                  // ...then square is valid candidate
                                                  candidates[candidateCount].row = blockRow;
                                                  candidates[candidateCount].col = blockColumn;
                                                  ++candidateCount;
                                             }
                                        }
                                   }

                                   // if there was only one possible square
                                   if (candidateCount == 1)
                                   {
                                        // populate the struct for returning values
                                        suggestion.location = *candidates;
                                        suggestion.newValue = digit;

                                        // recommend that the user fill that square with the digit
                                        if (verbose)
                                        {
                                             printf("Try changing square %c%d to %d\n",
                                                  colLabels[candidates->col],
                                                  candidates->row + 1, digit);
                                        }
                                   }
                              }
                         }
                    }
               }
          }
     }

     // when all cross-hatches have been processed, notify the user if we didn't find any suggestions
     if (!suggestion.newValue && verbose)
     {
          printf(sudokuAssistantNoSuggestionMessage);
     }

     return suggestion;
}

HistoryStep assistantLocked(SudokuBoard * board, bool verbose)
{
     HistoryStep suggestion = { 0 };
     DigitsPresent digitsPresent;
     SudokuDigitTestField digitsPossible[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT] = { 0 };
     SudokuDigitTestField testFlag;
     int row, column, block, digit;

     // make sure we know which digits are present in each row, column, and block
     evaluateDigitsPresent(board, &digitsPresent, false);

     // make initial analysis of possible digits for each square
     for (row = 0; row < SUDOKU_ROW_COUNT; ++row)
     {
          for (column = 0; column < SUDOKU_COL_COUNT; ++column)
          {
               // only process squares that have no digit already
               if (board->contents[row][column] == 0)
               {
                    // determine which block we're in currently
                    block = SUDOKU_BLOCK_FROM_INTERSECTION(row, column);

                    // initialize square with all digits possible
                    digitsPossible[row][column] = SUDOKU_TEST_ALLDIGITS;

                    // unset any digits PRESENT in the square's row
                    digitsPossible[row][column] &= ~digitsPresent.rows[row];

                    // unset any digits PRESENT in the square's column
                    digitsPossible[row][column] &= ~digitsPresent.columns[column];

                    // unset any digits PRESENT in the square's block
                    digitsPossible[row][column] &= ~digitsPresent.blocks[block];
               }
          }
     }

     // SHORTCUT: If any squares now have only 1 possible candidate, suggest that candidate
     suggestion.newValue = scanForSingleCandidate(digitsPossible, &suggestion.location);

     // if suggestion value is still 0, keep looking
     if (!suggestion.newValue)
     {
          // use "locked candidate rule" to eliminate candidates

          // There are two forms of the locked candidate rule, but they are logically equivalent;
          // checking with form #1 is the same as checking with form #2.

          // This program implements form #1:
          // "When a candidate is possible in a certain block and row/column, and it is not possible
          //  anywhere else in the same row/column, then it is also not possible anywhere else in the
          //  same block."
          // (source: https://www.stolaf.edu/people/hansonr/sudoku/explain.htm).

          // "lock" candidates by block, processing rows
          for (row = 0; row < SUDOKU_ROW_COUNT; ++row)
          {
               for (digit = 1; digit <= SUDOKU_DIGIT_MAX; ++digit)
               {
                    bool digitPossibleInBlock[3] = { 0 };
                    int i, possibleBlockCount = 0;
                    testFlag = SUDOKU_TEST_FLAG_SHIFT(digit);

                    for (column = 0; column < SUDOKU_COL_COUNT; ++column)
                    {
                         block = SUDOKU_BLOCK_FROM_INTERSECTION(row, column);

                         // if digit is possible in this square of current row...
                         if (digitsPossible[row][column] & testFlag)
                         {
                              // ...record which block-column it was possible in
                              digitPossibleInBlock[block % 3] = true;
                         }
                    }

                    // once all columns in current row have been broken down into their blocks,
                    // check if current digit is possible ONLY within one block
                    for (i = 0; i < 3; ++i)
                    {
                         if (digitPossibleInBlock[i])
                         {
                              ++possibleBlockCount;
                              block = i + ((row / 3) * 3);
                         }
                    }

                    if (possibleBlockCount == 1)
                    {
                         // unset the digitsPossible for squares inside selected bloock that are 
                         // OUTSIDE current row
                         int blockRow, blockColumn, blockRowOffset, blockColumnOffset;

                         blockRowOffset = (block / 3) * 3;

                         for (blockRow = blockRowOffset;
                         blockRow < SUDOKU_BLOCK_HEIGHT + blockRowOffset; ++blockRow)
                         {
                              blockColumnOffset = (block % 3) * 3;

                              for (blockColumn = blockColumnOffset;
                              blockColumn < SUDOKU_BLOCK_WIDTH + blockColumnOffset; ++blockColumn)
                              {
                                   // if current row is NOT the same as outer row...
                                   if (blockRow != row)
                                   {
                                        // ... remove current digit from square's digitsPossible
                                        digitsPossible[blockRow][blockColumn] &= ~testFlag;
                                   }
                              }
                         }
                    }
               }
          }

          // "lock" candidates by block, processing columns
          for (column = 0; column < SUDOKU_COL_COUNT; ++column)
          {
               for (digit = 1; digit <= SUDOKU_DIGIT_MAX; ++digit)
               {
                    bool digitPossibleInBlock[3] = { 0 };
                    int i, possibleBlockCount = 0;
                    testFlag = SUDOKU_TEST_FLAG_SHIFT(digit);

                    for (row = 0; row < SUDOKU_COL_COUNT; ++row)
                    {
                         block = SUDOKU_BLOCK_FROM_INTERSECTION(row, column);

                         // if digit is possible in this square of current column...
                         if (digitsPossible[row][column] & testFlag)
                         {
                              // ...record which block-row it was possible in
                              digitPossibleInBlock[block / 3] = true;
                         }
                    }

                    // once all rows in current column have been broken down into their blocks,
                    // check if current digit is possible ONLY within one block
                    for (i = 0; i < 3; ++i)
                    {
                         if (digitPossibleInBlock[i])
                         {
                              ++possibleBlockCount;
                              block = i * 3 + (column / 3);
                         }
                    }

                    if (possibleBlockCount == 1)
                    {
                         // unset the digitsPossible for squares inside selected bloock that are 
                         // OUTSIDE current column
                         int blockRow, blockColumn, blockRowOffset, blockColumnOffset;

                         blockRowOffset = (block / 3) * 3;

                         for (blockRow = blockRowOffset;
                         blockRow < SUDOKU_BLOCK_HEIGHT + blockRowOffset; ++blockRow)
                         {
                              blockColumnOffset = (block % 3) * 3;

                              for (blockColumn = blockColumnOffset;
                              blockColumn < SUDOKU_BLOCK_WIDTH + blockColumnOffset; ++blockColumn)
                              {
                                   // if current column is NOT the same as outer column...
                                   if (blockColumn != column)
                                   {
                                        // ... remove current digit from square's digitsPossible
                                        digitsPossible[blockRow][blockColumn] &= ~testFlag;
                                   }
                              }
                         }
                    }
               }
          }

          // Now that locked candidates have been identified, scan again
          suggestion.newValue = scanForSingleCandidate(digitsPossible, &suggestion.location);
     }

     // After all steps, if a suggestion was found, recommend it
     if (suggestion.newValue && verbose)
     {
          printf("Try changing square %c%d to %d\n",
               colLabels[suggestion.location.col],
               suggestion.location.row + 1, suggestion.newValue);
     }
     // Otherwise, notify the user that no suggestions were found
     else if (verbose)
     {
          printf("Sorry, no recommendations found using this assistant\n");
     }

     return suggestion;
}

char scanForSingleCandidate(SudokuDigitTestField digitsPossible[][SUDOKU_COL_COUNT], Coord2D *suggestedSquare)
{
     char suggestionValue = 0;
     bool suggestionFound = false;
     int row, column, block, digit, bit, bitFlagged, numberFound;

     // check all squares to see if only one candidate is possible
     for (row = 0; !suggestionFound && row < SUDOKU_ROW_COUNT; ++row)
     {
          for (column = 0; !suggestionFound && column < SUDOKU_COL_COUNT; ++column)
          {
               numberFound = 0;
               
               // count how many '1' bits are set for current square
               for (bit = 0; bit < SUDOKU_DIGIT_MAX; ++bit)
               {
                    // shift current square's SudokuDigitTestField a certain number of bits
                    // if the least-significant-bit is '1', that is a flagged value
                    if ((digitsPossible[row][column] >> bit) & 1)
                    {
                         // we found a flag, so increment the counter
                         ++numberFound;

                         // save the position of the set bit, in case we can recommend it
                         bitFlagged = bit;
                    }
               }

               // if numberFound is 1, then only one candidate exists for this square
               if (numberFound == 1)
               {
                    suggestionFound = true;
                    suggestedSquare->row = row;
                    suggestedSquare->col = column;

                    // since only one digit was flagged, 'bitFlagged' is safe to use
                    // add 1, because bitFlagged represents place-value, which starts at 0
                    suggestionValue = bitFlagged + 1;
               }
          }
     }

     // check if any block has only one square possible for a candidate
     for (digit = 1; !suggestionFound && digit <= SUDOKU_DIGIT_MAX; ++digit)
     {
          bit = SUDOKU_TEST_FLAG_SHIFT(digit);
          
          for (block = 0; !suggestionFound && block < SUDOKU_BLOCK_COUNT; ++block)
          {
               numberFound = 0;
               
               for (row = (block / SUDOKU_BLOCK_HEIGHT) * SUDOKU_BLOCK_HEIGHT;
                    !suggestionFound &&
                    row < (block / SUDOKU_BLOCK_HEIGHT) * SUDOKU_BLOCK_HEIGHT + SUDOKU_BLOCK_HEIGHT;
                    ++row)
               {
                    for (column = (block % SUDOKU_BLOCK_WIDTH) * SUDOKU_BLOCK_WIDTH;
                         !suggestionFound &&
                         column < (block % SUDOKU_BLOCK_WIDTH) * SUDOKU_BLOCK_WIDTH + SUDOKU_BLOCK_WIDTH;
                         ++column)
                    {
                         // count candidates for current digit inside current block
                         if (digitsPossible[row][column] & bit)
                         {
                              // found a candidate, so increment counter
                              ++numberFound;

                              // save the square, in case we can recommend it
                              suggestedSquare->row = row;
                              suggestedSquare->col = column;
                         }
                    }
               }

               // if only one candidate for that digit exists in current block...
               if (numberFound == 1)
               {
                    // ...we have a recommendation
                    suggestionFound = true;
                    suggestionValue = digit;
               }
          }
     }

     // if no recommendation was found, 'recommendationValue' is still 0 (i.e. false)
     // otherwise, function returns recommended value for square
     return suggestionValue;
}


const SudokuAssistant *matchAssistant(char *name)
{
     const SudokuAssistant *assistant = NULL;
     int i;

     for (i = 0; !assistant && i < SUDOKU_ASSISTANT_COUNT; ++i)
     {
          if (strcmp(name, assistants[i].name) == 0)
          {
               assistant = &assistants[i];
          }
     }

     return assistant;
}