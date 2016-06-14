/******************************************************************************
 * Program: sudoku_commands.c
 *
 * Purpose: Implements the menu-command system for users to interact with the 
 *          program
 *
 * Developer: Philip Ormand
 *
 * Date: 5/13/16
 *
 *****************************************************************************/
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "sudoku_commands.h"
#include "sudoku_test_digits.h"
#include "sudoku_assistant.h"
#include "sudoku_help.h"


#define IS_ANY_INPUT_REMAINING(inputPtr) input->currentIndex + 1 < input->string.length
#define GET_CHAR_FROM_INPUT_STRING(inputPtr, loopVar) inputPtr->string.array[loopVar + inputPtr->currentIndex]


// Forward declarations, so function names are visible for definition of 'commands' array
SudokuCommandResult commandNew(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandLoad(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandCheck(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandChange(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandAssist(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandSolve(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandDisplay(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandUndo(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandRedo(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandHelp(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandExit(SudokuBoard *board, SudokuCommandInput *input);
SudokuCommandResult commandCommands(SudokuBoard *board, SudokuCommandInput *input);


#define SUDOKU_COMMAND_COUNT sizeof(commands)/sizeof(*commands)

const struct SudokuCommand commands[] = {
     { "new", "Begin new sudoku game", "new <game-type>", SUDOKU_HELP_NEW, commandNew },
     { "load", "Load sudoku game from text file", "load <filename>", SUDOKU_HELP_LOAD, commandLoad },
     { "check", "Checks sudoku board to see if solution is correct", "check", SUDOKU_HELP_CHECK, commandCheck },
     { "change", "Change a square's value", "change <column-letter> <row-number> <digit>", SUDOKU_HELP_CHANGE, commandChange },
     { "assist", "Use an assistant to get suggestion", "assist <assistant-type>", SUDOKU_HELP_ASSIST, commandAssist },
     { "solve", "Let an assistant automatically fill as many squares as it can", "solve <assistant-type>", SUDOKU_HELP_SOLVE, commandSolve },
     { "display", "Displays the current state of the sudoku board", "display", SUDOKU_HELP_DISPLAY, commandDisplay },
     { "undo", "Undoes changes made to the board", "undo <number-of-steps>", SUDOKU_HELP_UNDO, commandUndo },
     { "redo", "Redoes changes that were undone", "redo <number-of-steps>", SUDOKU_HELP_REDO, commandRedo },
     { "help", "Offers details about a particular command", "help <command>", SUDOKU_HELP_HELP, commandHelp },
     { "exit", "Exit program", "exit", SUDOKU_HELP_EXIT, commandExit },
     { "commands", "Displays available commands", "commands", SUDOKU_HELP_COMMANDS, commandCommands },
};

const char *showAllCommandsPrompt = "Type 'commands' to list all available commands.";

struct SudokuBoardPreset
{
     char *name;
     char board[SUDOKU_ROW_COUNT][SUDOKU_COL_COUNT];
};

typedef struct SudokuBoardPreset SudokuBoardPreset;

#define SUDOKU_BOARDPRESET_NAME_LENGTH_MAX 16
#define SUDOKU_BOARDPRESET_COUNT sizeof(boardPresets)/sizeof(*boardPresets)

const SudokuBoardPreset boardPresets[] = {
     { "blank", { 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, } 
     },
     { "easy", { 0, 0, 3, 0, 4, 2, 0, 9, 0,
                 0, 9, 0, 0, 6, 0, 5, 0, 0,
                 5, 0, 0, 0, 0, 0, 0, 1, 0,
                 0, 0, 1, 7, 0, 0, 2, 8, 5,
                 0, 0, 8, 0, 0, 0, 1, 0, 0,
                 3, 2, 9, 0, 0, 8, 7, 0, 0,
                 0, 3, 0, 0, 0, 0, 0, 0, 1,
                 0, 0, 5, 0, 9, 0, 0, 2, 0,
                 0, 8, 0, 2, 1, 0, 6, 0, 0, } 
     },
     { "supereasy", { 7, 0, 5, 1, 0, 4, 8, 0, 6,
                      1, 0, 8, 0, 5, 0, 4, 9, 0,
                      4, 0, 3, 6, 2, 0, 0, 5, 7,
                      0, 7, 0, 0, 4, 2, 3, 8, 0,
                      0, 3, 0, 0, 1, 7, 6, 2, 9,
                      2, 5, 9, 3, 0, 0, 0, 0, 1,
                      3, 0, 0, 0, 0, 9, 5, 1, 8,
                      9, 1, 6, 8, 0, 5, 0, 0, 0,
                      0, 8, 2, 4, 7, 0, 9, 0, 3, } 
     },
     { "moderate", { 4, 0, 0, 9, 0, 3, 5, 0, 6,
                     2, 9, 1, 0, 4, 0, 0, 0, 0,
                     0, 0, 6, 0, 0, 1, 0, 0, 0,
                     0, 0, 0, 0, 0, 6, 3, 0, 7,
                     0, 0, 0, 0, 0, 0, 0, 0, 0,
                     6, 0, 2, 5, 0, 0, 0, 0, 0,
                     0, 0, 0, 7, 0, 0, 8, 0, 0,
                     0, 0, 0, 0, 6, 0, 9, 7, 1,
                     9, 0, 8, 1, 0, 4, 0, 0, 2, } 
     },
};

SudokuCommandResult commandNew(SudokuBoard *board, SudokuCommandInput *input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;
     const SudokuBoardPreset *preset = NULL;
     char presetName[SUDOKU_BOARDPRESET_NAME_LENGTH_MAX];
     int i;

     // if no argument was provided for preset name
     if (!getStringArgument(input, presetName, sizeof(presetName)))
     {
          // preset defaults to "blank"
          strcpy(presetName, "blank");
     }

     for (i = 0; !preset && i < SUDOKU_BOARDPRESET_COUNT; ++i)
     {
          if (strcmp(presetName, boardPresets[i].name) == 0)
          {
               preset = &boardPresets[i];
          }
     }

     // if match was found for the preset name provided
     if (preset)
     {
          // put sudoku board back to clean, default state
          initializeSudokuBoard(board);

          // copy board contents from preset into sudoku board
          copySudokuBoardContents(preset->board, board->contents);

          // display new state of the board
          printSudokuBoard(board);
     }
     // if no match was found
     else
     {
          printf("Sorry, no preset found with name \"%s\"\n", presetName);
          status = SUDOKU_COMMAND_FAILURE;
     }

     return status;
}

SudokuCommandResult commandLoad(SudokuBoard *board, SudokuCommandInput *input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;
     char fileName[FILENAME_MAX];

     // if argument provided for filename of sudoku input file
     if (getStringArgument(input, fileName, sizeof(fileName)))
     {
          // if filename exists and board was loaded successfully
          if (loadSudokuBoard(fileName, board))
          {
               printf("Successfully loaded sudoku board \"%s\"\n\n", fileName);
               
               // display new state of the board
               printSudokuBoard(board);
          }
          else
          {
               status = SUDOKU_COMMAND_FAILURE;
          }
     }
     // if no argument was provided, that's an invalid command
     else
     {
          status = SUDOKU_COMMAND_USAGE;
     }
     
     return status;
}

SudokuCommandResult commandCheck(SudokuBoard *board, SudokuCommandInput *input)
{
     struct DigitsPresent digitsPresent;

     evaluateDigitsPresent(board, &digitsPresent, true);
     
     return SUDOKU_COMMAND_SUCCESS;
}

SudokuCommandResult commandChange(SudokuBoard *board, SudokuCommandInput *input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;
     char column, row, value;

     // if there are arguments to read
     if (IS_ANY_INPUT_REMAINING(input))
     {
          // if any argument not present OR any arguments were invalid
          if (!getColumnArgument(input, &column) ||
              !getRowArgument(input, &row) ||
              !getSudokuDigitArgument(input, &value))
          {
               status = SUDOKU_COMMAND_USAGE;
          }
     }
     // no arguments were provided. Prompt for input
     else
     {
          promptForColumn(&column);
          promptForRow(&row);
          promptForSudokuDigit(&value);
     }

     // all input collected, regardless of method. If we're still okay, make changes to board
     if (status == SUDOKU_COMMAND_SUCCESS)
     {
          // all arguments read and validated
          int oldValue = board->contents[row][column];

          // create history item
          Coord2D square;
          square.col = column;
          square.row = row;
          addUndoStep(board->history, &square, value);

          // change square value
          board->contents[row][column] = value;

          // if there were undo steps past this point in the stack, they are now invalidated
          invalidateSubsequentRedoSteps(board->history);

          printf("Changed square %c%d from %d to %d\n\n",
               colLabels[column], row + 1, oldValue, value);

          // display new state of board
          printSudokuBoard(board);
     }

     return status;
}

SudokuCommandResult commandAssist(SudokuBoard *board, SudokuCommandInput *input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;

     const SudokuAssistant *assistant = NULL;
     char assistantName[SUDOKU_ASSISTANT_NAME_LENGTH_MAX];

     // if argument provided for type of assistant to use
     if (getStringArgument(input, assistantName, sizeof(assistantName)))
     {
          // if assistant exists
          if (assistant = matchAssistant(assistantName))
          {
               assistant->assistantFunction(board, true);
          }
          else
          {
               printf("Sorry, \"%s\" is not a valid assistant.\n", assistantName);
               status = SUDOKU_COMMAND_FAILURE;
          }
     }
     // if no argument was provided, that's an invalid command
     else
     {
          status = SUDOKU_COMMAND_USAGE;
     }

     return status;
}

SudokuCommandResult commandSolve(SudokuBoard * board, SudokuCommandInput * input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;

     const SudokuAssistant *assistant = NULL;
     char assistantName[SUDOKU_ASSISTANT_NAME_LENGTH_MAX];

     // if argument provided for type of assistant to use
     if (getStringArgument(input, assistantName, sizeof(assistantName)))
     {
          // if assistant exists
          if (assistant = matchAssistant(assistantName))
          {
               HistoryStep currentChange = assistant->assistantFunction(board, false);
               int i;

               // if we got at least one suggestion
               if (currentChange.newValue)
               {
                    // only need to invalidate subsequent history items ONCE
                    invalidateSubsequentRedoSteps(board->history);
               }
               
               // as long as assistant keeps returning suggestions:
               for (i = 1; currentChange.newValue; 
                    currentChange = assistant->assistantFunction(board, false), ++i)
               {
                    // create history item
                    addUndoStep(board->history, &currentChange.location, currentChange.newValue);

                    // change square value
                    board->contents
                         [currentChange.location.row]
                         [currentChange.location.col] = currentChange.newValue;

                    // let the user know what changes are being made
                    printf("%2d: Changed square %c%d to %d\n",
                         i, colLabels[currentChange.location.col], 
                         currentChange.location.row + 1, currentChange.newValue);
               }

               // if we got at least one suggestion...
               if (i > 1)
               {
                    // put gap between change-log and board-printout
                    putchar('\n');

                    // display present state of the board
                    printSudokuBoard(board);
               }
               // otherwise, let the user know
               else
               {
                    fputs(sudokuAssistantNoSuggestionMessage, stdout);
               }
          }
          else
          {
               printf("Sorry, \"%s\" is not a valid assistant.\n", assistantName);
               status = SUDOKU_COMMAND_FAILURE;
          }
     }
     // if no argument was provided, that's an invalid command
     else
     {
          status = SUDOKU_COMMAND_USAGE;
     }

     return status;
}

SudokuCommandResult commandDisplay(SudokuBoard *board, SudokuCommandInput *input)
{
     // display present state of the board
     printSudokuBoard(board);
     
     return SUDOKU_COMMAND_SUCCESS;
}

SudokuCommandResult commandUndo(SudokuBoard *board, SudokuCommandInput *input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;
     unsigned stepsToUndo;

     // if argument provided for stepsToUndo is missing or invalid, assume 1 undo step
     if (!getUnsignedArgument(input, &stepsToUndo))
     {
          stepsToUndo = 1;
     }

     // all arguments read and validated
     if (undoStep(board->history, stepsToUndo))
     {
          // display new state of board
          putchar('\n');
          printSudokuBoard(board);
     }
     else
     {
          // if undo fails, specific error message provided by restoreUndoStep
          status = SUDOKU_COMMAND_FAILURE;
     }

     return status;
}

SudokuCommandResult commandRedo(SudokuBoard *board, SudokuCommandInput *input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;
     unsigned stepsToRedo;

     // if argument provided for stepsToRedo is missing or invalid, assume 1 redo step
     if (!getUnsignedArgument(input, &stepsToRedo))
     {
          stepsToRedo = 1;
     }

     // all arguments read and validated
     if (redoStep(board->history, stepsToRedo))
     {
          // display new state of board
          putchar('\n');
          printSudokuBoard(board);
     }
     else
     {
          // if undo fails, specific error message provided by redoUndoStep
          status = SUDOKU_COMMAND_FAILURE;
     }

     return status;
}

SudokuCommandResult commandHelp(SudokuBoard *board, SudokuCommandInput *input)
{
     SudokuCommandResult status = SUDOKU_COMMAND_SUCCESS;
     const struct SudokuCommand *command = NULL;
     char commandName[SUDOKU_COMMAND_NAME_LENGTH_MAX];

     // if argument provided for command to help with
     if (getStringArgument(input, commandName, sizeof(commandName)))
     {
          // if command exists
          if (command = matchCommand(commandName))
          {
               printf("\"%s\": %s\n", command->name, command->description);
               printf("Usage: '%s'\n", command->usagePrompt);
               printf(command->helpText);
          }
          else
          {
               printf("Sorry, \"%s\" is not a valid command.\n", commandName);
               status = SUDOKU_COMMAND_FAILURE;
          }
     }
     // if no argument was provided, that's an invalid command
     else
     {
          status = SUDOKU_COMMAND_USAGE;
     }

     return status;
}

SudokuCommandResult commandExit(SudokuBoard *board, SudokuCommandInput *input)
{
     puts("Goodbye!");
     return SUDOKU_COMMAND_EXIT;
}

SudokuCommandResult commandCommands(SudokuBoard * board, SudokuCommandInput * input)
{
     printCommands();

     return SUDOKU_COMMAND_SUCCESS;
}

bool getStringArgument(SudokuCommandInput * input, char * argument, size_t maxLength)
{
     bool status = true;
     
     if (input)
     {
          // if the string's array is not NULL and it contains characters
          if (input->string.array && input->string.length > 0)
          {
               // maxLength gets 1 subtracted from it in the loop (to leave room for null-terminator)
               // this will cause unsigned overflow it maxLength == 0
               if (maxLength > 0)
               {
                    // if there is still input to read
                    if (IS_ANY_INPUT_REMAINING(input))
                    {
                         size_t i; // for comparison with maxLength, i should be unsigned type
                         int ch;

                         for (i = 0, ch = GET_CHAR_FROM_INPUT_STRING(input, i);
                              i < maxLength - 1 && ch != 0 && !isspace(ch);
                              ++i, ch = GET_CHAR_FROM_INPUT_STRING(input, i))
                         {
                              argument[i] = ch;
                         }

                         // null-terminate the argument
                         argument[i] = 0;

                         // skip to the end of current argument
                         while (!isspace(ch) && ch != 0)
                         {
                              ++i;
                              ch = input->string.array[i + input->currentIndex];
                         }

                         // skip the whitespace until next argument
                         while (isspace(ch))
                         {
                              ++i;
                              ch = input->string.array[i + input->currentIndex];
                         }

                         // i + currentIndex is now positioned over the next valid character of
                         // input from the input string.
                         input->currentIndex += i;
                    }
                    else
                    {
                         status = false;
                    }
               }
               else
               {
                    terminate("ERROR: tried to get string argument with a maxLength of 0");
               }
          }
          else
          {
               terminate("ERROR: tried to get string argument from an invalid String");
          }
     }
     else
     {
          terminate("ERROR: tried to get string argument from a null SudokuCommandInput");
     }

     return status;
}

bool getUnsignedArgument(SudokuCommandInput * input, unsigned * argument)
{
     bool status = true;
     
     if (input)
     {
          // if the string's array is not NULL and it contains characters
          if (input->string.array && input->string.length > 0)
          {
               // if there is still input to read
               if (IS_ANY_INPUT_REMAINING(input))
               {
                    String numberCharacters = { 0 };
                    initializeString(&numberCharacters);
                    
                    size_t i; // for comparison with maxLength, i should be unsigned type
                    int ch;

                    for (i = 0, ch = GET_CHAR_FROM_INPUT_STRING(input, i);
                         ch != 0 && !isspace(ch);
                         ++i, ch = GET_CHAR_FROM_INPUT_STRING(input, i))
                    {
                         // get only digits from the argument
                         if (isdigit(ch))
                         {
                              addCharToString(&numberCharacters, ch);
                         }
                    }

                    // skip the whitespace until next argument
                    while (isspace(ch))
                    {
                         ++i;
                         ch = input->string.array[i + input->currentIndex];
                    }

                    // i + currentIndex is now positioned over the next valid character of
                    // input from the input string.
                    input->currentIndex += i;

                    // convert the digit characters read into numeric form
                    // because we read only digits, number should never be negative
                    *argument = (unsigned)atoi(numberCharacters.array);

                    // NOTE: if conversion fails, *argument == 0
               }
               else
               {
                    status = false;
               }
          }
          else
          {
               terminate("ERROR: tried to get unsigned argument from an invalid String");
          }
     }
     else
     {
          terminate("ERROR: tried to get unsigned argument from a null SudokuCommandInput");
     }

     return status;
}

/**
 * returns the INTEGER value of the column, starting from 0, NOT the letter that was provided
 */
bool getColumnArgument(SudokuCommandInput * input, char *argument)
{
     bool status = true;
     
     if (input)
     {
          // if the string's array is not NULL and it contains characters
          if (input->string.array && input->string.length > 0)
          {
               // if there is still input to read
               if (IS_ANY_INPUT_REMAINING(input))
               {
                    int i = 0;

                    *argument = GET_CHAR_FROM_INPUT_STRING(input, i);
                    ++i;

                    // skip the whitespace until next argument, if any exists
                    while (isspace(GET_CHAR_FROM_INPUT_STRING(input, i)))
                    {
                         ++i;
                    }

                    // i + currentIndex is now positioned over the next valid character of
                    // input from the input string.
                    input->currentIndex += i;

                    // input read successfully, now validate it

                    // turn ASCII code into integer value
                    *argument = SUDOKU_COL_LETTER_TO_INDEX(*argument);

                    // is argument in range [0, 8] (inclusive)?
                    status = validateColIndex(*argument);
               }
               else
               {
                    puts("No argument provided for column letter");
                    status = false;
               }
          }
          else
          {
               terminate("ERROR: tried to get column argument from an invalid String");
          }
     }
     else
     {
          terminate("ERROR: tried to get column argument from a null SudokuCommandInput");
     }

     return status;
}

bool getRowArgument(SudokuCommandInput * input, char * argument)
{
     bool status = true;

     if (input)
     {
          // if the string's array is not NULL and it contains characters
          if (input->string.array && input->string.length > 0)
          {
               // if there is still input to read
               if (IS_ANY_INPUT_REMAINING(input))
               {
                    int i = 0;

                    *argument = GET_CHAR_FROM_INPUT_STRING(input, i);
                    ++i;

                    // skip the whitespace until next argument, if any exists
                    while (isspace(GET_CHAR_FROM_INPUT_STRING(input, i)))
                    {
                         ++i;
                    }

                    // i + currentIndex is now positioned over the next valid character of
                    // input from the input string.
                    input->currentIndex += i;

                    // input read successfully, now validate it

                    // turn ASCII code into integer value
                    *argument = SUDOKU_ROW_NUMBER_TO_INDEX(*argument);

                    // is argument inside range [0, 8] (inclusive)?
                    status = validateRowIndex(*argument);
               }
               else
               {
                    puts("No argument provided for row number");
                    status = false;
               }
          }
          else
          {
               terminate("ERROR: tried to get row argument from an invalid String");
          }
     }
     else
     {
          terminate("ERROR: tried to get row argument from a null SudokuCommandInput");
     }

     return status;
}

bool getSudokuDigitArgument(SudokuCommandInput * input, char * argument)
{
     bool status = true;

     if (input)
     {
          // if the string's array is not NULL and it contains characters
          if (input->string.array && input->string.length > 0)
          {
               // if there is still input to read
               if (IS_ANY_INPUT_REMAINING(input))
               {
                    int i = 0;

                    *argument = GET_CHAR_FROM_INPUT_STRING(input, i);
                    ++i;

                    // skip the whitespace until next argument, if any exists
                    while (isspace(GET_CHAR_FROM_INPUT_STRING(input, i)))
                    {
                         ++i;
                    }

                    // i + currentIndex is now positioned over the next valid character of
                    // input from the input string.
                    input->currentIndex += i;

                    // input read successfully, now validate it

                    // turn ASCII code into integer value
                    *argument = SUDOKU_DIGIT_CHAR_TO_VALUE(*argument);

                    // is argument in range [0, 9] (inclusive)?
                    status = validateSudokuDigit(*argument);
               }
               else
               {
                    puts("No argument provided for new sudoku digit");
                    status = false;
               }
          }
          else
          {
               terminate("ERROR: tried to get sudoku digit argument from an invalid String");
          }
     }
     else
     {
          terminate("ERROR: tried to get sudoku digit argument from a null SudokuCommandInput");
     }

     return status;
}

void printCommands()
{
     int i;

     puts("Available commands:");

     for (i = 0; i < SUDOKU_COMMAND_COUNT; ++i)
     {
          printf("'%s'   \t(%s)\n", commands[i].name, commands[i].description);
     }
}

const struct SudokuCommand *getCommand(SudokuCommandInput *input)
{
     const struct SudokuCommand *command = NULL;

     do
     {
          char commandName[SUDOKU_COMMAND_NAME_LENGTH_MAX];
          
          clearString(&input->string);
          input->currentIndex = 0;
          ensureCleanInput();

          fputs("Enter command: ", stdout);
          readString(&input->string);
          putchar('\n');

          if (getStringArgument(input, commandName, SUDOKU_COMMAND_NAME_LENGTH_MAX))
          {
               command = matchCommand(commandName);
          }

          // command will be null if getCommandNameArgument failed OR if no match was found
          if (!command)
          {
               printf("'%s' is not a valid command. \n%s\n\n", commandName, showAllCommandsPrompt);
          }
          
     } while (!command);

     return command;
}

const struct SudokuCommand *matchCommand(char *name)
{
     const struct SudokuCommand *command = NULL;
     int i;

     for (i = 0; !command && i < SUDOKU_COMMAND_COUNT; ++i)
     {
          if (strcmp(name, commands[i].name) == 0)
          {
               command = &commands[i];
          }
     }

     return command;
}
