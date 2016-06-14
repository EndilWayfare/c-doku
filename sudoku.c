/******************************************************************************
 * Program: sudoku.c
 *
 * Purpose: To implement a sudoku application, with assistant functions to help
 *          user solve sudoku puzzles
 *
 * Developer: Philip Ormand
 *
 * Date: 5/13/16
 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sudoku_board.h"
#include "sudoku_commands.h"
#include "sudoku_test_digits.h"
#include "sudoku_utility.h"

/*
 * ======= SUDOKU SEMANTICS =======   (AKA, my chosen definitions for terms in variable names, etc.)
 * 'board': the sudoku puzzle space. The actual matrix that makes up a given puzzle or solution.
 * 'square': individual space for a digit on a sudoku board. A sudoku board is 9 squares by 9 squares.
 * 'row' / 'column': self-explanitory. A horizontal or vertical row of squares on the sudoku board.
 * 'block' : the 9 3x3 blocks of squares that must each contain the digits 1-9 on the sudoku board.
 * 
 * Which block is which?
 *   +---+---+---+
 *   | 1 | 2 | 3 |
 *   +---+---+---+
 *   | 4 | 5 | 6 |
 *   +---+---+---+
 *   | 7 | 8 | 9 |
 *   +---+---+---+
 */

int main(int argc, char* argv[])
{
     bool running = true;
     struct SudokuBoard board = { 0 };
     struct SudokuCommandInput commandInput = { 0 };
     const struct SudokuCommand *command = NULL;
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;

     initializeSudokuBoard(&board);
     initializeString(&commandInput.string);

     // if one argument was provided, go ahead and load sudoku board from textfile
     if (argc > 1)
     {
          // if filename provided by argument cannot be found/read, loadSudokuBoard will
          // print an error statement, and execution will continue with a blank board
          loadSudokuBoard(argv[1], &board);
     }

     puts("========== Sudoku Game ==========");
     puts(showAllCommandsPrompt);
     putchar('\n');
     printSudokuBoard(&board);

     while (running)
     {          
          // put extra space between previous output and command prompt
          putchar('\n');

          command = getCommand(&commandInput);

          status = command->commandFunction(&board, &commandInput);

          if (status == SUDOKU_COMMAND_USAGE)
          {
               printf("Usage: '%s'\n", command->usagePrompt);
          }
          else if (status == SUDOKU_COMMAND_EXIT)
          {
               running = false;
          }
     }

     freeHistory(&board.history);

     return 0;
}


/*
int main(int argc, char **argv)
{
     char *fileName = "sudoku_text_file.txt";
     FILE *sudokuTextFile;
     struct SudokuBoard board;

     if (argc == 2)
     {
          // if an argument was provided
          fileName = argv[1];
     }

     initializeSudokuBoard(&board);

     if ( (sudokuTextFile = fopen(fileName, "r")) == NULL )
     {
          terminate("ERROR: couldn't open sudoku file");
     }

     if ( !readSudokuBoard(sudokuTextFile, &board) )
     {
          exit(EXIT_FAILURE); // error message printed by readSudokuBoard
     }

     printSudokuBoard(&board);
     putchar('\n');

     puts("Evaluating solution:");
     evaluateDigitsPresent(&board);

     exit(EXIT_SUCCESS);
}
*/