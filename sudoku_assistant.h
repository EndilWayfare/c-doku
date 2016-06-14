#ifndef SUDOKU_ASSISTANT_H
#define SUDOKU_ASSISTANT_H

#include "sudoku_commands.h"
#include "sudoku_undo.h"

#define SUDOKU_ASSISTANT_NAME_LENGTH_MAX 16

struct SudokuAssistant
{
     char *name;
     char *description;
     HistoryStep(*assistantFunction)(struct SudokuBoard *board, bool verbose);
};

typedef struct SudokuAssistant SudokuAssistant;

extern const SudokuAssistant assistants[];

extern const char *sudokuAssistantNoSuggestionMessage;

const SudokuAssistant *matchAssistant(char *name);

#endif // SUDOKU_ASSISTANT_H

