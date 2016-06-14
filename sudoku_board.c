/******************************************************************************
 * Program: sudoku_board.c
 *
 * Purpose: Defines struct describing the state of a sudoku puzzle board, along
 *          with functions for initializing, loading, copying, displaying and 
 *          freeing the board's contents
 *
 * Developer: Philip Ormand
 *
 * Date: 5/13/16
 *
 *****************************************************************************/
#include <ctype.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sudoku_board.h"
#include "sudoku_test_digits.h"
#include "sudoku_undo.h"


void initializeSudokuBoard(struct SudokuBoard *board)
{
     // allocate new history stack for this fresh board
     if (board->history)
     {
          // if there is an existing history, free it so it's not leaked
          freeHistory(&board->history);
     }

     board->history = createHistory(board);

     // initialize board contents to 0;
     memset((void*)&(board->contents), 0, SUDOKU_ROW_COUNT * SUDOKU_COL_COUNT);
}

void freeSudokuBoardResources(struct SudokuBoard *board)
{
     freeHistory(&board->history);
}

bool loadSudokuBoard(char *fileName, struct SudokuBoard* board)
{
     FILE *file;

     if (file = fopen(fileName, "r"))
     {
          // reset sudoku board
          initializeSudokuBoard(board);
          
          char *currentSquare = board->contents[0],
               *boardEnd = board->contents[0] + SUDOKU_ROW_COUNT * SUDOKU_COL_COUNT,
               input;  // we can use char for input instead of int, because we're checking feof()

          while (currentSquare < boardEnd && !feof(file))
          {
               // scan file stream until we encounter a digit
               while (!isdigit(input = getc(file)) && !feof(file)) // break if EOF is encountered
               {
                    ;
               }

               if (!feof(file))
               {
                    // convert ASCII digit to binary integer by subtracting the ASCII code for 0
                    *currentSquare++ = input - '0';
               }
          }
     }
     else {
          printf("Sorry, file \"%s\" not found\n", fileName);
          // return false instead of terminating. program might try to recover.
          return false;
     }     

     return true;
}

void copySudokuBoardContents(const char source[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT], char destination[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT])
{
     const char *currentSourceSquare = source[0],
          *sourceEnd = currentSourceSquare + SUDOKU_ROW_COUNT * SUDOKU_COL_COUNT;
     char *currentDestinationSquare = destination[0];

     while (currentSourceSquare < sourceEnd)
     {
          *currentDestinationSquare++ = *currentSourceSquare++;
     }
}

void printSudokuBoard(struct SudokuBoard *board)
{
     // strings, not #defines: reduce size of compiled code (due to macro expansions)
     // static variables: only one copy of each string exists across all calls
     static char *rowDivider = "     ++---+---+---++---+---+---++---+---+---++\n",
                 *rowDividerThick = "     ++===+===+===++===+===+===++===+===+===++\n",
                 *rowFormatString = " %d - || %d | %d | %d || %d | %d | %d || %d | %d | %d ||\n",
                 *xAxisLabel = "        A   B   C    D   E   F    G   H   I\n";
     int i, j;

     fputs(xAxisLabel, stdout);
     for (i = 0; i < SUDOKU_ROW_COUNT; ++i)
     {
          if ( (i % 3) == 0 )
          {
               fputs(rowDividerThick, stdout);
          }
          else
          {
               fputs(rowDivider, stdout);
          }

          // row label
          printf(" %d - ", i + 1);

          for (j = 0; j < SUDOKU_COL_COUNT; ++j)
          {
               // print "thick" border if on a block boundary
               if ( (j % 3) == 0 )
               {
                    fputs("||", stdout);
               }
               else
               {
                    putchar('|');
               }

               // print blank if 0, otherwise print digit
               if (board->contents[i][j] > 0)
               {
                    printf(" %d ", board->contents[i][j]);
               }
               else
               {
                    fputs("   ", stdout);
               }
          }

          fputs("||\n", stdout);
     }
     fputs(rowDividerThick, stdout);
     
}