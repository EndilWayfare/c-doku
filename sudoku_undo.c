/******************************************************************************
 * Program: sudoku.c
 *
 * Purpose: To implement a history system that keeps track of changes made to
 *          a sudoku board over time. Allows program to undo or redo steps.
 *
 * Developer: Philip Ormand
 *
 * Date: 5/13/16
 *
 *****************************************************************************/
#include <stdlib.h>

#include "sudoku_board.h"
#include "sudoku_undo.h"
#include "sudoku_utility.h"

// internal structs defined in source file, not header file, for information-hiding
// purposes. "clients" of the History ADT don't need to know how it's implemented,
// only the functions they can call to make use of its functionality

/**
* Struct representing the history of a sudoku board
*/
struct HistoryStruct {
     HistoryStep *steps;           /**< dynamically allocated array of HistorySteps */
     HistoryStep *currentStep;     /**< off-the-end pointer to top of History stack */
     size_t length;                /**< number of steps currently in history */
     size_t capacity;              /**< number of elements allocated in 'steps' */

     struct SudokuBoard *owner;    /**< whose history is this? So we don't have to pass
                                        it as a parameter to any "member" functions */
};

typedef struct HistoryStruct HistoryStruct;


/**
 * Factory function that creates and initializes a dynamic History object for a 
 * SudokuBoard. The SudokuBoard calls this function with itself as a parameter
 * and stores the return value in its history member.
 *
 * @param owner SudokuBoard that will own the created History stack
 */
History createHistory(struct SudokuBoard *owner)
{
     // allocate space for a HistoryStruct
     History history = malloc(sizeof(*history));
     
     if (history == NULL) // failed to allocate HistoryStruct
     {
          terminate("ERROR: could not create History");
     }

     // allocate a dynamic array for holding HistorySteps
     history->capacity = 1;
     history->length = 0;
     history->steps = malloc(sizeof(HistoryStep) * history->capacity);

     if (history->steps == NULL)
     {
          terminate("ERROR: could not initialize History steps");
     }

     // position currentStep to be ready for recording the first history event
     history->currentStep = history->steps;

     // record owner of this History stack for future undo/redo operations
     history->owner = owner;

     return history;
}

/**
* Frees a dynamically-allocated HistoryStruct to prevent memory leaks.
* Pointer passed to function will be set to NULL at the end, indicating that it was freed.
*
* @param historyPtr Pointer to HistoryStruct to be freed
*/
void freeHistory(History *historyPtr)
{
     if (historyPtr && *historyPtr)
     {
          History history = *historyPtr;
          
          // deallocate memory used to store all the HistorySteps
          free(history->steps);

          // deallocate the History object itself
          free(history);

          // make pointer null, so it can't be accidently freed again
          *historyPtr = NULL;
     }
     else
     {
          terminate("ERROR: tried to free a null History pointer");
     }
}

/**
* Reallocates a HistoryStruct's 'steps' member with a larger size if there not enough room to add
* the specified number of HistorySteps
* No change occurs if capacity is large enough already.
*
* @param history Pointer to HistoryStruct that will be modified
* @param stepsToAdd Number of additional HistorySteps to ensure space for
*/
void growHistory(History history, size_t stepsToAdd)
{
     if (history)
     {
          if (history->steps)
          {
               if (stepsToAdd > (history->capacity - history->length))
               {
                    int currentStepIndex = history->currentStep - history->steps;
                    history->capacity *= 2;
                    history->steps = realloc(history->steps, sizeof(HistoryStep) * history->capacity);

                    if (history->steps == NULL)
                    {
                         terminate("ERROR: unable to grow history");
                    }

                    history->currentStep = history->steps + currentStepIndex;
               }
          }
          else
          {
               terminate("ERROR: tried to grow a History with null steps array");
          }
     }
     else
     {
          terminate("ERROR: tried to grow a null History");
     }
}

/**
* Add a HistoryStep to a HistoryStruct stack
*
* @param history Pointer to HistoryStruct that will receive step
* @param square Row/column address of the change
* @param value New value for the square
*/
void addUndoStep(History history, Coord2D *square, int value)
{
     if (history)
     {
          if (validateSudokuCoord2D(square))
          {
               if (validateSudokuDigit(value))
               {
                    // store location and current value of sudoku square
                    history->currentStep->location = *square;
                    history->currentStep->newValue = value;
                    history->currentStep->oldValue =
                         history->owner->contents[square->row][square->col];

                    // advance history
                    ++history->currentStep;
                    ++history->length;
                    growHistory(history, 1);
               }
               else
               {
                    terminate("ERROR: tried to add undo step with invalid sudoku digit");
               }
          }
          else
          {
               terminate("ERROR: tried to add undo step with invalid square address");
          }
     }
     else
     {
          terminate("ERROR: tried to add undo step to null History");
     }
}

/**
* Roll back the changes associated with the most recent HistoryStep(s) in a HistoryStruct
* Automatically recognizes if bottom of the history stack is reached.
*
* @param history Pointer to HistoryStruct
* @param stepsToUndo Number of HistorySteps to roll back
* @return True if any steps were undone, false if no steps were undone
*/
bool undoStep(History history, size_t stepsToUndo)
{
     bool status = true;

     if (history)
     {
          if (history->currentStep)
          {
               // only take action if the number of steps is not 0
               if (stepsToUndo)
               {
                    // if currentStep is the bottom of the stack, there are no steps to undo
                    if (history->currentStep == history->steps)
                    {
                         puts("<no steps to undo>");
                         status = false;
                    }
                    else
                    {                        
                         Coord2D *currentSquare = &(history->currentStep - 1)->location;
                         size_t i, existingValue;

                         printf("Undoing %d steps:\n", stepsToUndo);

                         for (i = 0; i < stepsToUndo && history->currentStep != history->steps; ++i)
                         {
                              --history->currentStep;
                              currentSquare = &history->currentStep->location;
                              existingValue = history->owner->contents[currentSquare->row][currentSquare->col];
                              history->owner->contents[currentSquare->row][currentSquare->col] =
                                   history->currentStep->oldValue;

                              printf("   %d: changed %c%d from %d back to %d\n", i + 1,
                                   colLabels[currentSquare->col], currentSquare->row + 1,
                                   existingValue, history->currentStep->oldValue);
                         }

                         // if function was asked to undo more steps than were available
                         if (i < stepsToUndo)
                         {
                              puts("<no more steps to undo>");
                         }
                    }
               }
          }
          else
          {
               terminate("ERROR: can't restore undo step if currentStep is null");
          }
     }
     else
     {
          terminate("ERROR: tried to restore undo step from a null History");
     }

     return status;
}

/**
* Reapplies the changes associated with the most recent undone HistoryStep(s) in a HistoryStruct
* Automatically recognizes when the top of history stack is reached.
*
* @param history Pointer to HistoryStruct
* @param stepsToUndo Number of HistorySteps to reapply
* @return True if any steps were redone, false if no steps were redone
*/
bool redoStep(History history, size_t stepsToRedo)
{
     bool status = true;

     if (history)
     {
          if (history->currentStep)
          {
               // only take action if the number of steps is not 0
               if (stepsToRedo)
               {
                    // if currentStep is the top of the stack, there are no steps to redo
                    if (history->currentStep == history->steps + history->length)
                    {
                         puts("<no steps to redo>");
                         status = false;
                    }
                    else
                    {
                         Coord2D *currentSquare = &(history->currentStep - 1)->location;
                         size_t i, existingValue;

                         printf("Redoing %d steps:\n", stepsToRedo);

                         for (i = 0; i < stepsToRedo && history->currentStep != history->steps + history->length; ++i)
                         {
                              currentSquare = &history->currentStep->location;
                              existingValue = history->owner->contents[currentSquare->row][currentSquare->col];
                              history->owner->contents[currentSquare->row][currentSquare->col] =
                                   history->currentStep->newValue;

                              printf("   %d: changed %c%d from %d back to %d\n", i + 1,
                                   colLabels[currentSquare->col], currentSquare->row + 1,
                                   existingValue, history->currentStep->newValue);

                              ++history->currentStep;
                         }

                         // if function was asked to undo more steps than were available
                         if (i < stepsToRedo)
                         {
                              puts("<no more steps to redo>");
                         }
                    }
               }
          }
          else
          {
               terminate("ERROR: can't restore undo step if currentStep is null");
          }
     }
     else
     {
          terminate("ERROR: tried to restore undo step from a null History");
     }

     return status;
}

/**
* If there are any HistorySteps PAST the current history location (i.e. steps that could be redone),
* make them inaccessible, because they are no longer valid.
*
* @param history Pointer to HistoryStruct
*/
void invalidateSubsequentRedoSteps(History history)
{
     // set current state of board as final available redo step
     history->length = history->currentStep - history->steps;
}
