#ifndef SUDOKU_UTILITY_H
#define SUDOKU_UTILITY_H

#include <stdlib.h>

#define SUDOKU_ROW_COUNT 9
#define SUDOKU_COL_COUNT 9
#define SUDOKU_BLOCK_COUNT 9
#define SUDOKU_BLOCK_WIDTH 3
#define SUDOKU_BLOCK_HEIGHT 3
#define SUDOKU_DIGIT_MAX 9

/** turn ASCII code into integer, subtracting 1 to convert the human-readable column number to an array index */
#define SUDOKU_COL_LETTER_TO_INDEX(letter) toupper(letter) - 'A'
/** turn column index integer value back into ASCII */
#define SUDOKU_COL_INDEX_TO_LETTER(index) (index) + 'A'
/** turn ASCII code into integer, subtracting 1 to convert the human-readable row number to an array index */
#define SUDOKU_ROW_NUMBER_TO_INDEX(number) (number) - '0' - 1;
/** turn row index integer value back into ASCII */
#define SUDOKU_ROW_INDEX_TO_NUMBER(index) (index) + '0' + 1
/** turn ASCII code into integer */
#define SUDOKU_DIGIT_CHAR_TO_VALUE(character) (character) - '0'
/** turn sudoku digit integer value back into ASCII */
#define SUDOKU_DIGIT_VALUE_TO_CHAR(digit) (digit) + '0'
/** given a row and column, find which block contains the intersection. Numbering begins at 0 */
#define SUDOKU_BLOCK_FROM_INTERSECTION(row, column) ((row) / 3) * 3 + (column) / 3


extern const char colLabels[];


void ensureCleanInput();

bool hasStdinBeenRead(bool wasJustRead);


void promptForColumn(char *index);

void promptForRow(char *index);

void promptForSudokuDigit(char *digit);


bool validateRowIndex(size_t row);

bool validateColIndex(size_t col);

bool validateSudokuDigit(size_t digit);


/**
* Struct representing the coordinates of a 2-Dimensional point
* (Origin of coordiate system is top-left)
*/
struct Coord2D {
     /** 
      * Union of anonymous structs. 
      * allows 'Coord2D.col' and 'Coord2D.x' to mean the same thing
      */
     union {
          struct {
               size_t col; /**< horizontal coordinate */
               size_t row; /**< vertical coordinate */
          };
          struct {
               size_t x; /**< horizontal coordinate */
               size_t y; /**< vertical coordinate */
          };
     };
};

typedef struct Coord2D Coord2D;

bool validateSudokuCoord2D(Coord2D *coord);


/**
 * Dynamically-allocated c-string ADT with a growth strategy similar to C++'s vector class
 * 
 * If remaining capacity is not enough to hold the characters a user wants to add, the array
 * is reallocated with double the current capacity.
 *
 * ADT currently supports only adding a character at a time, to allow reading from text input
 * like STDIN. No other functionality presently necessary, as the ADT is only used for reading
 * command + argument strings.
 */
struct String {
     char *array;     /**< dynamically-allocated char array to hold contents */
     size_t length;   /**< number of characters currently in string. INCLUDES null-terminator */
     size_t capacity; /**< number of elements allocated in 'array' */
};

typedef struct String String;

void initializeString(String *string);

void freeString(String *string);

char addCharToString(String *string, char ch);

int readString(String *string);

void clearString(String *string);

void terminate(const char *message);

#endif // !SUDOKU_UTILITY_H
