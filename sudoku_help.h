#ifndef SUDOKU_HELP_H
#define SUDOKU_HELP_H

#define SUDOKU_HELP_NEW \
"\nClears current sudoku board and replaces it with a preset.\n" \
"Default preset (when no argument is provided) is a blank sudoku board.\n" \
"\nArguments:\n" \
"   - <game-type>: Name of a preset board to start from:\n" \
"         - \"blank\": A blank sudoku board\n"

#define SUDOKU_HELP_LOAD \
"\nDigits may be separated by commas, spaces, any delimiting character, or nothing at all. " \
"Digits are loaded into the board from left to right, starting at the top-left square\n" \
"If there is a zero in the file, the corresponding square will be treated as blank. " \
"If there are not enough digits in the file to fill the whole sudoku board, the remaining " \
"squares will be blank. If there are too many digits in the file, additional digits will " \
"be ignored once the board is full. Nonsense characters that don't correspond to digits " \
"will be automatically ignored.\n" \
"\nArguments:\n" \
"   - <filename>: Name of the text file to load\n"

#define SUDOKU_HELP_CHECK \
"\nDisplays feedback if any problems are encountered.\n"

#define SUDOKU_HELP_CHANGE \
"\nUsing a value of '0' means the square will be blank.\n" \
"\nArguments:\n" \
"   - <column-letter>: Letter identifying the column of the square\n" \
"   - <row-number>: Number identifying the row of the square\n" \
"   - <digit>: New value for the square\n"

#define SUDOKU_HELP_ASSIST \
"\nAn assistant is a helper that looks for possible additions to the board based on the " \
"digits that are already present.\n" \
"\nArguments:\n" \
"   - <assistant-type>: Name of the assistant to use:\n" \
"         - \"crosshatch\": Uses cross-hatch scanning to identify 'hidden singles'\n" \
"         - \"locked\": Uses row/column and block exclusion to 'lock' candidates\n"

#define SUDOKU_HELP_SOLVE \
"\nAutomatically applies the suggestions of an assistant until no more suggestions are available. " \
"This will either solve the sudoku puzzle or exhaust the help of the given assistant.\n" \
"\nArguments:\n" \
"   - <assistant-type>: Name of the assistant to use:\n" \
"         - \"crosshatch\": Uses cross-hatch scanning to identify 'hidden singles'\n" \
"         - \"locked\": Uses row/column and block exclusion to 'lock' candidates\n"

#define SUDOKU_HELP_DISPLAY \
""

#define SUDOKU_HELP_UNDO \
"\nEvery time you use the 'change' command to alter a square, an undo step is created. \n" \
"Calling the command with no arguments rolls back a single undo step, as long as there " \
"are steps to undo.\n" \
"\nArguments:\n" \
"   - <number-of-steps>: Number of undo steps to roll back\n"

#define SUDOKU_HELP_REDO \
"\nIf you used the 'undo' command to roll back changes, the 'redo' command makes the changes " \
"again.\n" \
"\nArguments:\n" \
"   - <number-of-steps>: Number of steps to redo\n"

#define SUDOKU_HELP_HELP \
"\nThis is the command you're using right now.\n" \
"\nArguments:\n" \
"   - <command>: Name of the command to get help with\n"

#define SUDOKU_HELP_EXIT \
"\nCheerfully bids you adieu, until the next time you desire to tangle with the mathematical " \
"and logical arcana of sudoku\n"

#define SUDOKU_HELP_COMMANDS \
""

#endif // !SUDOKU_HELP_H
