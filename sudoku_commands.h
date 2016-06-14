#ifndef SUDOKU_COMMANDS_H
#define SUDOKU_COMMANDS_H

#include <stdbool.h>

#include "sudoku_board.h"
#include "sudoku_utility.h"

#define SUDOKU_COMMAND_NAME_LENGTH_MAX 16

enum SudokuCommandResult {
     SUDOKU_COMMAND_SUCCESS,
     SUDOKU_COMMAND_USAGE,
     SUDOKU_COMMAND_FAILURE,
     SUDOKU_COMMAND_EXIT,
};
typedef enum SudokuCommandResult SudokuCommandResult;

struct SudokuCommandInput
{
     String string;
     size_t currentIndex;
};

typedef struct SudokuCommandInput SudokuCommandInput;

struct SudokuCommand
{
     char *name;
     char *description;
     char *usagePrompt;
     char *helpText;
     SudokuCommandResult(*commandFunction)(SudokuBoard *board, SudokuCommandInput *input);
};

typedef struct SudokuCommmand SudokuCommand;

extern const struct SudokuCommand commands[];

extern const char *showAllCommandsPrompt;


bool getStringArgument(SudokuCommandInput *input, char *argument, size_t maxLength);

bool getUnsignedArgument(SudokuCommandInput *input, unsigned *argument);

bool getColumnArgument(SudokuCommandInput *input, char *argument);

bool getRowArgument(SudokuCommandInput *input, char *argument);

bool getSudokuDigitArgument(SudokuCommandInput *input, char *argument);


void printCommands();

const struct SudokuCommand *getCommand(SudokuCommandInput *input);

const struct SudokuCommand *matchCommand(char *name);

#endif // !SUDOKU_COMMANDS_H