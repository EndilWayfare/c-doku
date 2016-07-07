/******************************************************************************
 * Program: sudoku.c
 *
 * Purpose: Defining miscellaneous data structs, validation functions, and
 *          constants
 *
 * Developer: Philip Ormand
 *
 * Date: 5/13/16
 *
 *****************************************************************************/
#include <ctype.h>
#include <memory.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "sudoku_utility.h"


 /**
  * Array containing the letters 'A' through 'I'.
  * Retreiving an item from this array with a column index will return the corresponding letter 
  * of that column
  */
const char colLabels[] = "ABCDEFGHI";


/**
 * Calling this function guarantees that the subsequent read will be at the beginning of STDIN
 * or at the beginning of a new line of input.
 * (no isolated '\n' at the beginning of input, which might throw off the subsequent read)
 */
void ensureCleanInput()
{
     // if stdin is at EOF, either it hasn't been read from yet, or we're on an implementation
     // that sets EOF when 'Enter' is pressed at the end of a read.
     // hasStdinBeenRead returns true if it has ever been called with 'true' as an argument
     // we did not just read from stdin, so we call it with 'false'. 
     if (!feof(stdin) && hasStdinBeenRead(false))
     {
          // otherwise, we've got to clear it to the next newline
          char ch = 0;
          while ((ch = getchar()) != EOF && ch != '\n') {  }

          // last char read was EOF or '\n'. We're clean!
     }
}

/**
 * Is STDIN clean, having never been read from since program start?
 * Calling this function with 'false' says "I'm not using STDIN, I just want to know if it is clean";
 * Calling this function with 'true' says "I read from STDIN, so it's not guaranteed to be clean anymore".
 * Until function is called with 'true', return will always be 'false'.
 * After calling with 'true', subsequent calls to the function will always return 'true'.
 *
 * @param wasJustRead 'true' if program read from STDIN, 'false' if just checking status
*/
bool hasStdinBeenRead(bool wasJustRead)
{
     static bool hasBeenRead = false;

     // if hasBeenRead == false && wasJustRead == false, hasBeenRead will still be false
     // the first time the function is called with wasJustRead == true, hasBeenRead will == true
     // from there, function will always return true.
     hasBeenRead |= wasJustRead;

     return hasBeenRead;
}


/**
* Displays prompt asking user to input a column letter.
* Function returns the ARRAY INDEX for a sudoku column (NOT the ASCII letter)
*
* @param index Pointer to a char that will contain the result
*/
void promptForColumn(char *index)
{
     do
     {
          fputs("Enter a column letter: ", stdout);

          ensureCleanInput();
          hasStdinBeenRead(true);
          scanf(" %c", (char*) index);
          *index = SUDOKU_COL_LETTER_TO_INDEX(*index);

     } while (!validateColIndex(*index));
}

/**
* Displays prompt asking user to input a row number.
* Function returns the ARRAY INDEX for a sudoku row (begins at 0, not 1)
*
* @param index Pointer to a char that will contain the result
*/
void promptForRow(char *index)
{
     do
     {
          fputs("Enter a row number: ", stdout);

          ensureCleanInput();
          hasStdinBeenRead(true);
          scanf(" %c", (char*) index);
          *index = SUDOKU_ROW_NUMBER_TO_INDEX(*index);

     } while (!validateRowIndex(*index));
}

/**
* Displays prompt asking user to input a sudoku digit.
* Function returns the actual integer value (NOT the ASCII code for the number's symbol).
* A value of 0 represents a blank square.
*
* @param digit Pointer to a char that will contain the result
*/
void promptForSudokuDigit(char * digit)
{
     do
     {
          fputs("Enter a sudoku digit: ", stdout);

          ensureCleanInput();
          hasStdinBeenRead(true);
          scanf(" %c", (char*) digit);
          *digit = SUDOKU_DIGIT_CHAR_TO_VALUE(*digit);

     } while (!validateSudokuDigit(*digit));
}

/**
* Ensures that an index is within the range of a sudoku row.
*
* @param row Index to be validated
* @return True if valid, false if invalid
*/
bool validateRowIndex(size_t row)
{
     // is index in range [0, 8] (inclusive)?
     if (row >= 0 && row < SUDOKU_ROW_COUNT)
     {
          return true;
     }
     else
     {
          char asciiValue = (char) SUDOKU_ROW_INDEX_TO_NUMBER(row);
          printf("Sorry, '%d' (character '%c') is not a valid row number\n",
               asciiValue, asciiValue);
          return false;
     }
}

/**
* Ensures that an index is within the range of a sudoku column.
*
* @param column Index to be validated
* @return True if valid, false if invalid
*/
bool validateColIndex(size_t column)
{
     // is index in range [0, 8] (inclusive)?
     if (column >= 0 && column < SUDOKU_COL_COUNT)
     {
          return true;
     }
     else
     {
          printf("Sorry, '%c' is not a valid column letter\n",
               SUDOKU_COL_INDEX_TO_LETTER(column));
          return false;
     }
}

/**
* Ensures that a digit is a valid sudoku digit.
*
* @param digit Digit to be validated
* @return True if valid, false if invalid
*/
bool validateSudokuDigit(size_t digit)
{
     // is index in range [0, 9] (inclusive)?
     if (digit >= 0 && digit <= SUDOKU_DIGIT_MAX)
     {
          return true;
     }
     else
     {
          char asciiValue = (char) SUDOKU_DIGIT_VALUE_TO_CHAR(digit);
          printf("Sorry, '%d' (character '%c') is not a valid sudoku digit\n",
               asciiValue, asciiValue);
          return false;
     }
}

/**
* Ensures that a Coord2D struct's coordinates are in the correct range of a sudoku board.
*
* @param coord Pointer to struct that will be validated
* @return True if valid, false if invalid
*/
bool validateSudokuCoord2D(Coord2D * coord)
{
     return validateColIndex(coord->col) &&
          validateRowIndex(coord->row);
}

/**
* Initializes a String ADT with a new dynamically-allocated char array and default member values
*
* @param string Pointer to String that will be initialized
*/
void initializeString(struct String *string)
{
     if (string)
     {
          if (string->array != NULL)
          {
               // if, by some chance, we try to initialize a string that was already
               // initialized, make sure to free the old array, so it is not leaked
               free(string);
          }
          
          string->array = calloc(1, sizeof(char));
          if (string->array == NULL)
          {
               terminate("ERROR: could not create String");
          }

          string->capacity = 1;
          string->length = 1;
     }
     else
     {
          terminate("ERROR: tried to initialize a null String pointer");
     }
}

/**
* Frees resources of a String ADT to prevent memory leaks
* Member pointing to the dynamic char array is set to NULL, signifying the string is freed
*
* @param string Pointer to String that will be freed
*/
void freeString(struct String *string)
{
     if (string)
     {
          // if string's array is NULL, you can't free that.
          if (string->array)
          {
               free(string->array);
               string->array = NULL;
          }

          // even if the array is NULL, zero out the other members just in case
          string->length = 0;
          string->capacity = 0;
     }
     else
     {
          terminate("ERROR: tried to free a null String pointer");
     }
}

/**
* Reallocates dynamic char array with a larger size if there is not enough room to add the number
* of characters specified. No change occurs if String's capacity is large enough already.
*
* @param string Pointer to String that will be modified
* @param charsToAdd Number of additional characters to ensure space for
*/
void growString(struct String *string, size_t charsToAdd)
{
     if (string)
     {
          if (string->array)
          {
               if (charsToAdd > (string->capacity - string->length))
               {
                    string->capacity *= 2;
                    string->array = realloc(string->array, string->capacity);

                    if (string->array == NULL)
                    {
                         terminate("ERROR: unable to grow string");
                    }
               }
          }
          else
          {
               terminate("ERROR: tried to grow a String with a null array");
          }
     }
     else
     {
          terminate("ERROR: tried to grow a null String");
     }
}

/**
* Adds a single character to a String ADT.
* Calls 'growString' automatically to make sure there is enough room
*
* @param string Pointer to String receiving characters
* @param ch Character to add to String
*/
char addCharToString(struct String * string, char ch)
{
     if (string)
     {
          if (string->array)
          {
               growString(string, 1);
               string->array[string->length - 1] = ch;
               string->array[string->length++] = 0;
          }
          else
          {
               terminate("ERROR: tried to add a character to a String with a null array");
          }
     }
     else
     {
          terminate("ERROR: tried to add character to a null String");
     }

     return ch;
}

/**
* Reads STDIN, saving characters into a String ADT.
* Reads until newline/EOF is encountered
*
* @param string Pointer to String that will receive input
*/
int readString(struct String * string)
{
     int ch, charsAdded = 0;

     // we are about to read from stdin, so let hasStdinBeenRead know that, yes, stdin has been read
     hasStdinBeenRead(true);

     while ((ch = getchar()) == '\n')
     {
          // If the first character we read is a newline, that's just the residue from a
          // previous input. (This is the pattern established by scanf, so all my functions
          // follow the pattern to ensure maximum compatibility.)

          // If the first character encountered is NOT a newline, the loop will terminate, and
          // ch holds the first character of real input.
          // Otherwise, the loop will run again. Assuming the character following the first
          // newline is not another newline, the loop will terminate then. Regardless, this 
          // loop should chew through any number of consecutive newlines and leave ch with the
          // correct value: real input.
     }

     while (ch != EOF && ch != '\n')
     {
          addCharToString(string, ch);
          ++charsAdded;

          // ch was primed with the first character outside this loop.
          // this statement reads ch for NEXT iteration:
          ch = getchar();
     }

     if (ch == '\n')
     {
          // put back '\n' as a courtesy to the next function that tries to read input.
          // if last character read is EOF, this is not neccessary.
          ungetc(ch, stdin);
     }

     // null-termnate the string
     addCharToString(string, 0);

     // return number of chars added to string 
     return charsAdded;
}

/**
* Makes a String ADT empty, ready to receive new contents.
* Dynamic char array is NOT reallocated; String retains whatever capacity it had before function call.
*
* @param string Pointer to String that will be cleared
*/
void clearString(struct String * string)
{
     if (string)
     {
          if (string->array)
          {
               // first character null-terminated
               string->array[0] = 0;
               // length back to 1
               string->length = 1;
          }
          else
          {
               terminate("ERROR: tried to clear a String with a null array");
          }
     }
     else
     {
          terminate("ERROR: tried to clear a null String");
     }
}


/**
* Displays error message and exits irregularly if the program encounters a fatal error
*
* @param message Error message to display
*/
void terminate(const char *message)
{
     puts(message);
     exit(EXIT_FAILURE);
}

