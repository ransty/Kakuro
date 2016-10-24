#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFont>
#include <string>
#include <QBrush>
#include <QColor>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <sstream>



//both boards use the same number standards:

//-1 = answer cell - Initially,
//after calculations of sums, it is store in the following format
//horizontal sums are multiplied by 1000 and vertical sums are multiplied by 10 both values are added if in the same cell
//E.g.  5000 will be horizontal sum 5
//      160 will be vertical sum 16
// if in the same cell, it will be 5160
//Another E.g. 20220 means 20 - horizontal and 22 - vertical
//See MainWindow::populateAnswerCell() to see how this works

//-2 = grey cell

//0 = blank cell

//1-9 = filled in cell


//board holds the current values of the visual game board
std::vector<std::vector<double>> board;

//boardSolution contains the numbers that are needed to solve the board
//these numbers are never visible to the user
std::vector<std::vector<double>> boardSolution;

//the board layout
QStandardItemModel *model;
//creates a new cell
QStandardItem *cell;
QFont f("Smooth Sizes");

MainWindow::MainWindow(QWidget	*parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // srand used to create random numbers
    srand(time(NULL));

    ui->setupUi(this);
    // Linking mouse click to slot menuRequest
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(menuRequest(QPoint)));
    // Disable editing text using keyboard in the table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

MainWindow::~MainWindow()
{
   delete ui;
}

//uses the board vector to draw the board to the screen
void MainWindow::drawBoard(){
     model = new QStandardItemModel(board.size(), board[0].size(), this);
    // set the table view to the model
    ui->tableView->setModel(model);
    //loops through each cell in the board array
    for(int i = 0; i<(int) board.size(); i++){
        for(int j = 0; j<(int)board[i].size(); j++){

            // Answer cell
            if(board[i][j] >= 10){
                //sets all formatting for an answer cell
                f.setPointSize(9); // sets font size
                cell = new QStandardItem();
                QBrush brush = QBrush(QColor(Qt::blue));
                cell->setBackground(brush);
                cell->setFlags(Qt::NoItemFlags);
                QString answer = ""; // Sums will be appended here

                // For information on how both sums are stored in the vectors check populateAnswerCells() method

                // if not a horizontal sum
                if (board[i][j]/1000 < 1) {
                    answer += "\n\n";
                } else {
                    // Horizontal sum
                    std::stringstream convert; // Used for converting int to string
                    convert << floor(board[i][j]/1000); // Get horizontal sum using Math.floor()
                    answer = QString::fromStdString(convert.str()); // Append the converted int to the answer QString
                    answer += "\n\n"; // Some spaces to differentiate between a horizontal and vertical some
                }
                // Check if there is a vertical sum
                if (floor((int)(board[i][j]/10)%100) != 0){
                    std::stringstream convert; // for converting int to string
                    convert << floor((int)(board[i][j]/10)%100);
                    answer += QString::fromStdString(convert.str());
                }
                // Set the QString to the cell
                // FYI: All of the prior settings were lost because u created a new object
                // cell = new QStandardItem(answer);
                // Easy fix dw lads
                cell->setText(answer);
            }
            // Blank cell
            else if(board[i][j] == 0){
                f.setPointSize(9); // sets font size
                // Creates a cell with a string representation numbers 1-9
                cell = new QStandardItem(QString("1 2 3\n4 5 6\n7 8 9"));
            }
            // Filled in cell
            else if(board[i][j]>=1 && board[i][j]<=9){
                //valid number 1-9
                f.setPointSize(30); // sets font size
                //gives the cell a number from the board vector
                cell = new QStandardItem(QString::number(board[i][j]));
            }
            // Grey cell
            else if(board[i][j] <= -2){
                //sets all formatting for a grey cell
                cell = new QStandardItem();
                QBrush brush = QBrush(QColor(Qt::gray));
                cell->setBackground(brush);
                cell->setFlags(Qt::NoItemFlags);

            }
            else{
                //should never reach
                cell = new QStandardItem();
            }

            //a bit of formatting for the cell
            cell->setFont(f);
            cell->setTextAlignment(Qt::AlignCenter);


            //sets the cell into the model
            model->setItem(i, j, cell);

            // Prints out the solution FOR TESTING PURPOSES
            std::cout << boardSolution[i][j] << ", " << std::flush;
        }
        std::cout << std::endl;
    }

}

/**
 * @brief MainWindow::setBoardSize Sets the size of both vectors and clears them
 * @param height number of rows in the board
 * @param width number of colums in the board
 */
void MainWindow::setBoardSize(int height, int width){
    // Empty the boards, this is needed if the generate button is pressed multiple times
    board = {};
    boardSolution = {};

    //pretty simple,
    //just sets the size of the multidimentional array
    board.resize(height);
    boardSolution.resize(height);
    for(int i = 0; i<height; i++){
        board[i].resize(width);
        boardSolution[i].resize(width);
    }
}

//populates the board with random numbers
void MainWindow::populateBoardRandom(){

    //loops through each row
    for(int i = 0; i<(int)boardSolution.size(); i++){
        //loops through each column in each row
        for(int j = 0; j<(int)boardSolution[i].size(); j++){
            // If not a grey cell or answer cell
            if(boardSolution[i][j] >= 0){
                //generate random number between 1 and 9
                int randNum = rand()%(9-1 + 1) + 1;
                //places the random number in the current board cell
                boardSolution[i][j] = randNum;
            }
        }
    }  
}

/**
 * @brief MainWindow::populateAnswerCells sums up the horizontal and vertical sums
 *  and updates the boardSolution vector to contain the sums
 */
void MainWindow::populateAnswerCells() {

    //loops through each row
    for(int i = 0; i<(int)boardSolution.size(); i++){
        //loops through each column in each row
        for(int j = 0; j<(int)boardSolution[i].size(); j++){

            // for horizontal sums
            // Don't calculate sums if it is the last column
            if(j < ((int)boardSolution[i].size()-1)){
                // Check whether the current cell is grey and the next one on the right is a blank
                if(boardSolution[i][j] == -2 && boardSolution[i][j+1] > 0){
                    // Index of the next blank cells
                    int colIndex = j + 1;
                    // Sum for the column
                    int colSum = 0;
                    // Set the values of the cell back to 0 (from -1)
                    boardSolution[i][j] = 0;
                    // Loop until the cell on the right is not a grey cell
                    while(colIndex < (int)boardSolution[i].size() && boardSolution[i][colIndex] > 0){
                        // Add the values of all the blank cells
                        colSum += boardSolution[i][colIndex];
                        colIndex++;
                    }
                    // Multiply the horizontal sum by 1000
                    // This is because in some cases, both horizontal and vertical sums may be stored in one place
                    // To get back the original sum value for a horizontal cell, just divide the current value by 1000
                    // and remove the decimal point if any by rounding off using Math.floor()
                    colSum = colSum*1000;
                    boardSolution[i][j] = colSum;
                }
            }
            // for vertical sums
            // Don't calculate sums if it is the last row
            if(i < ((int)boardSolution.size()-1)){
                // Check whether the current cell is grey or a cell with horizontal sum, and whether the cell below is blank
                if((boardSolution[i][j] == -2 || boardSolution[i][j] >= 1000) && boardSolution[i+1][j] > 0){
                    if (boardSolution[i][j] == -2){
                        // If the cell is blank it will have a value of -2, so we need to make it 0 first
                        boardSolution[i][j] = 0;
                    }
                    // Index of blank cells below
                    int rowIndex = i + 1;
                    // Sum of the row
                    int rowSum = 0;
                    // Loop until the cell below is not a grey cell
                    while(rowIndex < (int)boardSolution.size() && boardSolution[rowIndex][j] > 0){
                        // Add the values of all the blank cells below
                        rowSum += boardSolution[rowIndex][j];
                        rowIndex++;
                    }
                    // Multiply the vertical sum by 10
                    // This is because in some cases, both horizontal and vertical sums may be stored in one place
                    // To get back the original sum value for a vertical cell, just divide it by 10 then modulo % 100
                    // and remove the decimal points if any by rounding off using Math.floor()
                    rowSum = rowSum * 10;
                    boardSolution[i][j] += rowSum;
                }
            }
        }
    }
}

void MainWindow::createRandomLayout(){
    //unimplemented at the moment

    //places random grey squares to start with
    //loops through each row
    for(int i = 0; i<(int)boardSolution.size(); i++){
        //loops through each column in each row
        for(int j = 0; j<(int)boardSolution[i].size(); j++){
            // All grey for top row and left column
            if(i == 0 || j == 0){
                board[i][j] = -2;
                boardSolution[i][j] = -2;
            } else {
                int chance = 3;
                //creates a random number
                int randNum = rand()%(chance-1 + 1) + 1;
                if(randNum == 1){
                    board[i][j] = -2;
                    boardSolution[i][j] = -2;
                }
            }
        }
    }



    //need to now make sure there are no unreachable squares

    //start with squares with 0 non-grey neighbours

    //loops through each row
    for(int i = 1; i<(int)boardSolution.size()-1; i++){
        //loops through each column in each row
        for(int j = 1; j<(int)boardSolution[i].size()-1; j++){
            if(board[i][j] == 0  && board[i+1][j] == -2 && board[i-1][j] == -2 && board[i][j+1] == -2 && board[i][j-1] == -2){
                board[i][j] = -2;
                boardSolution[i][j] = -2;
            }
        }
        //the reason for checking this seperately is to avoid trying j+1 when there is no such cell
        //check last cell in row
        if(board[i][board[i].size()-1] == 0 && board[i-1][board[i].size()-1] == -2 && board[i+1][board[i].size()-1] == -2 && board[i][board[i].size()-2] == -2){
            board[i][board[i].size()-1] = -2;
            boardSolution[i][board[i].size()-1] = -2;
        }


    }
    //check last row
    for(int i = 1; i<(int)board[0].size()-1; i++){
        if(board[board.size()-1][i] == 0 && board[board.size()-1][i+1] == -2 && board[board.size()-1][i-1] == -2 && board[board.size()-2][i]){
            board[board.size()-1][i] = -2;
            boardSolution[board.size()-1][i] = -2;


        }

    }

}

void MainWindow::createBlankBoardFromSolution(){
    // this is where we copy all the grey cells and answer cells from boardSolution to board

    //loops through each row
    for(int i = 0; i<(int)boardSolution.size(); i++){
        //loops through each column in each row
        for(int j = 0; j<(int)boardSolution[i].size(); j++){
            if(boardSolution[i][j] == -2){
                board[i][j] = -2;
            }
            else if(boardSolution[i][j] >= 100){
                board[i][j] = boardSolution[i][j];
            }
            else{
                board[i][j] = 0;
            }
        }
    }
}


//populates the board from a file
void MainWindow::populateBoardFromFile(){

    //opens a dialog box for the user to select a file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Text Files (*.txt)"));

    //opens the file
    QFile file(fileName);

    //calls the validateFile function
    if (validateFile(&file) == false){
        //if it fails to validate,
        //the program will ask if they want to select a new file to try

        //creating the message box
        QMessageBox msg;
        msg.setText("Failed. Would you like to try again with a new file?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setDefaultButton(QMessageBox::Yes);
        int ret = msg.exec();
        //simple check to see what they chose
        if(ret == QMessageBox::Yes){
            populateBoardFromFile();
        }
        else{
            return;
        }
    }
    else{
        //file is valid

        //sets the file back to the first character
        file.seek(0);

        //first need to find how big the grid will be
        //this whole section is to do so
        //this might be able to be condensed to something easier

        int width = 0;
        int height = 0;

        QTextStream in(&file);

        //loops through each line in the file
        while (!in.atEnd()) {
            //each line means there is another row of cells
            height ++;
            //reads the line
            QString line = in.readLine();

            //using a stringstream to seperate the values from the commas
            std::stringstream ss(line.toStdString());
            std::string token;
            width = 0;
            //loops through each number
            while (getline(ss,token, ','))
            {
                width++;
            }
        }
        //sets the board to the correct size
        setBoardSize(height, width);

        //sets the file back to the first character
        file.seek(0);

        //counters for current line and column
        int lineNumber = 0;
        int columnNumber = 0;

        //loops through each line
        while (!in.atEnd()) {
            //reads the nect line
            QString line = in.readLine();

            //splits the line up again
            std::stringstream ss(line.toStdString());
            std::string token;
            while (getline(ss,token, ','))
            {
                //actually enters the data into the arrays now
                board[lineNumber][columnNumber] = QString::fromStdString(token).toDouble();
                columnNumber++;
            }
            columnNumber = 0;
            lineNumber++;
        }
    }
    drawBoard();

}


//validates if a file is okay to be used
bool MainWindow::validateFile(QFile* file){
    //safety check for file
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

    QTextStream in(file);
    int lineLength = 0;
    while (!in.atEnd()) {
       QString line = in.readLine();

       //check all lines are the same length
       //first line
       if(lineLength == 0){
           lineLength = line.count(",");
       }
       else{
           //line is not the same length as the last line
           if(lineLength!=line.count(",")){
               //failed
               return false;
           }
       }


       //checks if the current line contains anything but numbers and commas
       QRegExp r("^-?[0-9](,-?[0-9])*$");
       if(line.contains(r)){

       }
       else{
           //failed
           return false;
       }
    }


//made it to the end of testing
return true;
}


void MainWindow::on_RandomNumbers_clicked()
{
    //set up size of array
    setBoardSize(ui->heightSpinBox->value(), ui->widthSpinBox->value());
    //create random layout
    createRandomLayout();
    //populates the boardSolution vector
    populateBoardRandom();
    //Adds up the horizontal and vertical sums
    populateAnswerCells();
    //populates the board vector
    createBlankBoardFromSolution();
    //calculate solutions
    //calculateSolutions();
    //draws the board to the screen
    drawBoard();

}

void MainWindow::on_saveFileButton_clicked()
{
    //-----------------
    //currently broken
    //needs to save with numbers from

    //opens save file dialog
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Image"), "",tr("Text Files (*.txt)"));

    //check if the file name is empty
    if(fileName.isEmpty())
        return;
    else
    {
        //creates the file if there was a file name
        QFile file(fileName);
        //checks if the file can be opened in write only mode
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            //displays an error message
            QMessageBox::information(this, tr("unable to open file"),file.errorString());
            return;
        }

        //creates a QTextStream object to write to the file
        QTextStream out(&file);

        //loops through each cell in the board array
        for(int i = 0; i<(int) board.size(); ++i){
            for(int j = 0; j<(int)board[i].size(); ++j)
            {
                // writes the value to the file
                out << board[i][j];

                // writes a comma after a value or adds a new line
                if (j != (int)board[i].size()-1)
                    out << ',';
                else
                    out << '\n';
            }
        }

        //flushes the file and closes it
        file.flush();
        file.close();
    }
}

void MainWindow::on_loadFileButton_clicked()
{
    populateBoardFromFile();
}

void MainWindow::menuRequest(QPoint pos)
{
    // Retrieving the row and column of the mouse click on the grid
    QModelIndex index = ui->tableView->indexAt(pos);
    // If the cell is a blank cell (excludes sum cells, grey cells)
    if (board[index.row()][index.column()] == 0) {
        // Create a menu item
        QMenu menu(this);
        // Create actions for the menu
        QAction *clearValue;
        QAction *setValue1;
        QAction *setValue2;
        QAction *setValue3;
        QAction *setValue4;
        QAction *setValue5;
        QAction *setValue6;
        QAction *setValue7;
        QAction *setValue8;
        QAction *setValue9;
        // Set text for the action and add them to the menu
        clearValue = menu.addAction("Clear set value");
        setValue1 = menu.addAction("Set value to 1");
        setValue2 = menu.addAction("Set value to 2");
        setValue3 = menu.addAction("Set value to 3");
        setValue4 = menu.addAction("Set value to 4");
        setValue5 = menu.addAction("Set value to 5");
        setValue6 = menu.addAction("Set value to 6");
        setValue7 = menu.addAction("Set value to 7");
        setValue8 = menu.addAction("Set value to 8");
        setValue9 = menu.addAction("Set value to 9");

        // Action that was clicked on
        QAction *action = menu.exec(ui->tableView->viewport()->mapToGlobal(pos));

        if (!action) {
            // User didn't press any QAction
            // Close the menu
        } else {
        // If it is a set action
        if (action->text().contains("Set value to ")) {
            // Create a new cell with the selected number
            cell = new QStandardItem(action->text().right(1));
            // Update the board with the selected number
            board[index.row()][index.column()] = action->text().right(1).toInt();
            // Change the font size
            f.setPointSize(30);
        // If it was clearValue action
        } else if (action->text().contains("Clear")) {
            // Create a cell with the default string for blank cell
            cell = new QStandardItem(QString("1 2 3\n4 5 6\n7 8 9"));
            //update the cell in the board vector
            board[index.row()][index.column()] = 0;
            // change the font size
            f.setPointSize(9);
        }
        cell->setFont(f);
        cell->setTextAlignment(Qt::AlignCenter);
        model->setItem(index.row(), index.column(), cell);
    }
    }
}
