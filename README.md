TODO:

1. Get rid of random.
1.a) Remove QAction items, use menu.
2. Save and load files using both vectors
3. Add a few already generated, valid boards.
4. Only show possible choices - making sure to adhere to both parts 3 and 4 of the minimal program list
5. Keep track of changes to the grid values using a data structure holding new objects called move.cpp. Add an undo button
6. Win solution. Proper checking of all sum cells to see if they are correct. Not comparing to boardSolution!
7. Once win solution occurs; let the user 'replay' the way the solution was found.
8. Task 6 - save moves to text file
9. Fix load game function


GRAPHICAL:
1. Sums need to be displayed in a better way on the board. Currently the top number is for the horizontal sum and the bottom one is for vertical sums.
    - maybe put a 2x2 table in each sumCell
 

OPTIONAL:
1. Highlight cells that have only one possible value - maybe change background color to yellow
2. Have an option to remove non-possible selections in every cell