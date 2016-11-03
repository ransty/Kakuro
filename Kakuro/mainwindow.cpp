#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "usermove.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFont>
#include <queue>
#include <stack>
#include <string>
#include <fstream>
#include <QBrush>
#include <QTime>
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
#include <qdebug.h>




//both boards use the same number standards:

//-1 = answer cell - Initially,
//after calculations of sums, it is stored in the following format
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

std::vector<userMove> moves;

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
    //srand(time(NULL));

    ui->setupUi(this);
    // Linking mouse click to slot menuRequest
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(menuRequest(QPoint)));
    // Disable editing text using keyboard in the table
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //Add items to the boardSize combo box
    ui->boardSize->addItem("Board Size");
    ui->boardSize->addItem("5x5");
    ui->boardSize->addItem("9x8");
    ui->boardSize->addItem("15x15");

    showingSolution = false;
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
            std::cout << board[i][j] << ", " << std::flush;
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

    //pretty simple,
    //just sets the size of the multidimentional array
    board.resize(height);
    boardSolution.resize(height);
    //boardSolution.resize(height);
    for(int i = 0; i<height; i++){
        board[i].resize(width);
        boardSolution[i].resize(width);
        //boardSolution[i].resize(width);
    }
}

void MainWindow::populateBoard(int height, int width){
     //3 Different boards
        if (height == 5 && width == 5){
            boardSolution = {
                    {-2,-2,100,290,-2},
                    {-2,6170,1,5,30},
                    {22000,9,3,8,2},
                    {18000,8,2,7,1},
                    {-2,13000,4,9,-2} };

        }
        else if (height == 9 && width == 8){
            boardSolution = {
                    {-2,-2,260,80,-2,210,140,-2},
                    {-2,11000,9,2,16040,7,9,-2},
                    {-2,17030,1,6,3,2,5,-2},
                    {9000,2,7,10000,1,9,160,-2},
                    {6000,1,5,220,4000,3,1,160},
                    {-2,12000,4,8,110,12000,3,9},
                    {-2,-2,15130,6,9,9110,2,7},
                    {-2,21000,5,1,2,9,4,-2},
                    {-2,15000,8,7,8000,2,6,-2} };

        }
        else if (height == 15 && width == 15){
            boardSolution = {
                    {-2,-2,40,60,-2,-2,-2,-2,70,160,-2,60,300,-2,-2},
                    {-2,4000,1,3,160,-2,-2,8230,1,7,8040,1,7,-2,-2},
                    {-2,14000,3,2,9,240,32000,6,4,9,3,2,8,-2,-2},
                    {-2,-2,16000,1,7,8,10040,8,2,13000,1,3,9,40,30},
                    {-2,-2,-2,-2,19290,7,3,9,160,160,-2,9000,6,1,2},
                    {-2,-2,40,15160,5,9,1,9350,2,7,170,-2,4000,3,1},
                    {-2,18000,3,7,8,-2,27000,7,3,9,8,100,-2,-2,-2},
                    {-2,17000,1,9,7,30,10000,9,1,11000,9,2,40,160,-2},
                    {-2,-2,-2,11000,9,2,14030,8,6,-2,13000,1,3,9,-2},
                    {-2,30,160,-2,12000,1,2,5,4,170,12060,4,1,7,-2},
                    {8000,1,7,100,-2,7000,1,6,12240,8,1,3,-2,-2,-2},
                    {14000,2,9,3,60,160,-2,20230,8,9,3,40,240,-2,-2},
                    {-2,-2,10000,2,1,7,15030,6,9,13000,2,3,8,40,-2},
                    {-2,-2,30000,1,3,9,2,8,7,-2,11000,1,7,3,-2},
                    {-2,-2,6000,4,2,10000,1,9,-2,-2,-2,10000,9,1,-2} };
        }
}

/*
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
*/


/**
 * @brief MainWindow::populateAnswerCells sums up the horizontal and vertical sums
 *  and updates the boardSolution vector to contain the sums
 */
/*
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
*/

/*
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
*/

void MainWindow::createBlankBoardFromSolution(){
    // this is where we copy all the grey cells and answer cells from boardSolution to board

    //loops through each row
    for(int i = 0; i<(int)boardSolution.size(); i++){
        //loops through each column in each row
        for(int j = 0; j<(int)boardSolution[i].size(); j++){
            if(boardSolution[i][j] == -2){
                board[i][j] = -2;
            }
            else if(boardSolution[i][j] >= 10){
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

    //CURRENTLY ASSUME FILE IS VALID
    //OLD VALIDATION CODE BELOW

    //opens a dialog box for the user to select a file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    
    // If the file path is empty (they clicked cancel)
    if (fileName == "") {
        return;
    }
    
    // Loads the file
    QFile file(fileName);
    file.open(QIODevice::ReadWrite);

    //sets the file back to the first character
    file.seek(0);

    QTextStream in(&file);


    //reads the whole file into a stringstream
    std::stringstream ss(in.readAll().toStdString());
    std::string token;

    //strings to hold each part of the file
    std::string sizeString;
    QString boardSolutionString;
    QString boardString;
    std::string movesString;

    int loopNumber = 0;
    //splits the file into the 4 vectors
    while(getline(ss, token, '*')){
        if(loopNumber == 0){
            sizeString = token;}
        else if(loopNumber == 1){
            boardSolutionString = QString::fromStdString(token);
        }
        else if(loopNumber == 2){
            boardString = QString::fromStdString(token);
        }
        else{
            movesString = token;
        }

        loopNumber++;
    }


    /////////////////////////
    //start sorting of size//
    /////////////////////////

    std::vector<std::string> sizeVector;
    std::stringstream sizeSs(sizeString);
    std::string sizeToken;
    while(getline(sizeSs, sizeToken, ',')){
        sizeVector.push_back(sizeToken);
    }


    //sets the board size to the data in the first vector
    setBoardSize(std::atoi(sizeVector[0].data()),std::atoi(sizeVector[1].data()));

    ////////////////////////////
    //finished sorting of size//
    ////////////////////////////


    //////////////////////////////////
    //start sorting of boardSolution//
    //////////////////////////////////
    QTextStream boardSolutionStream(&boardSolutionString);

    int boardSolutionColumn = 0;
    int boardSolutionLine = 0;

    while (!boardSolutionStream.atEnd()) {
        //reads the next line
        QString line = boardSolutionStream.readLine();


        //splits the line up again
        std::stringstream boardSolutionSs(line.toStdString());
        std::string boardSolutionToken;
        while (getline(boardSolutionSs,boardSolutionToken, ','))
        {

            //actually enters the data into the arrays now
            //have no idea why I need to do lineNumber - 1
            boardSolution[boardSolutionLine-1][boardSolutionColumn] = QString::fromStdString(boardSolutionToken).toDouble();
            boardSolutionColumn++;
        }
        boardSolutionColumn = 0;
        boardSolutionLine++;
    }

    /////////////////////////////////////
    //finished sorting of boardSolution//
    /////////////////////////////////////

    //////////////////////////
    //start sorting of board//
    //////////////////////////
    QTextStream boardStream(&boardString);

    int columnNumber = 0;
    int lineNumber = 0;

    while (!boardStream.atEnd()) {
        //reads the nect line
        QString line = boardStream.readLine();


        //splits the line up again
        std::stringstream boardSs(line.toStdString());
        std::string boardToken;
        while (getline(boardSs,boardToken, ','))
        {

            //actually enters the data into the arrays now
            //have no idea why I need to do lineNumber - 1
            board[lineNumber-1][columnNumber] = QString::fromStdString(boardToken).toDouble();
            columnNumber++;
        }
        columnNumber = 0;
        lineNumber++;
    }

    /////////////////////////////
    //finished sorting of board//
    /////////////////////////////


    //////////////////////////
    //start sorting of moves//
    //////////////////////////

    //need to clear the current move board
    moves.clear();

    //does our normal looping through a string and stopping at each ,
    std::stringstream moveSs(movesString);
    std::string moveToken;

    //need this test to remove whitespace line
    while (getline(moveSs,moveToken, '{'))
    {
        //checks if the line is valid
        if(moveToken.find('}') == 7){
            //gets the values in the format 1,1,1,1
            moveToken = moveToken.substr(0, 7);


            //variables to store the data in
            int rowTemp;
            int columnTemp;
            int oldTemp;
            int newTemp;

            //counter to know which value it is up to
            int count = 0;

            //splits the string into individual integers
            std::stringstream boardSs2(moveToken);
            std::string boardToken2;
            while (getline(boardSs2,boardToken2, ','))
            {
                //puts the current value into the correct temp integer
                if(count==0)
                    rowTemp = std::atoi(boardToken2.c_str());
                else if(count==1)
                    columnTemp = std::atoi(boardToken2.c_str());
                else if(count==2)
                    oldTemp = std::atoi(boardToken2.c_str());
                else
                    newTemp = std::atoi(boardToken2.c_str());
                count++;
            }

            //Now create userMove
            userMove myMove(rowTemp, columnTemp, oldTemp, newTemp);
            // Push the move
            moves.push_back(myMove);
        }


    }

    /////////////////////////////
    //finished sorting of moves//
    /////////////////////////////

    drawBoard();


    /*
    if (file.isReadable()) {
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
    */
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

void MainWindow::saveBoard() {
    // Opens save file dialog - Fine
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"), "",tr("Text Files (*.txt)"));

    // Check if the file name is empty - Fine
    if(fileName.isEmpty())
        return;
    else if (board.size() == 0 || boardSolution.size() == 0) {
        return;
    } else {
        // Creates the file if there was a file name - fine
        QFile file(fileName);
        // Checks if the file can be opened in write only mode - Fine
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // Displays an error message - Fine
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            return;
        }

        // Creates a QTextStream object to write to the file - Fine
        QTextStream out(&file);

        //std::string filePath = fileName.toUtf8().constData();

        // Save board size
        out << board.size() << "," << board[0].size();

        // Seperate size from board by *
        out << "\n*\n";

        // Now populate with answer board
        for (int i = 0; i < (int)boardSolution.size(); i++) {
            for (int j = 0; j < (int)boardSolution[i].size(); j++) {
                // Write current value to file
                out << boardSolution[i][j];

                // Writes a comma after a value or adds a new line
                if (j != (int)board[i].size()-1)
                    out << ',';
                else
                    out << '\n';
            }
        }

        // Seperate answer from board by *
        out << "*\n";


        for(int i = 0; i<(int) board.size(); ++i){
            for(int j = 0; j<(int)board[i].size(); ++j)
            {
                // Writes the value to the file
                out << board[i][j];

                // Writes a comma after a value or adds a new line
                if (j != (int)board[i].size()-1)
                    out << ',';
                else
                    out << '\n';
            }
        }

        // Seperate board from moves by *
        out << "*\n";

        // Now print the moves
        for (auto i : moves) {
            out << i.toString();
        }


        // Flushes the file and closes it
        file.flush();
        file.close();
    }
}

/**
 * @brief MainWindow::checkSection checks a section of the puzzle to see if it adds up to the corresponding sum
 * @param sum Vertical/Horizontal sum of the current section
 * @param y first cell in the section
 * @param x first cell in the section
 * @param yDelta change in y for moving to the next cell in the section
 * @param xDelta change in x for moving to the next cell in the section
 * @return true if the section is correct
 */
bool MainWindow::checkSection(int sum, int y, int x, int yDelta, int xDelta){

    // True if board[y][x] is in the current section
    bool inSection = true;
    int total = 0; // Total for the section
    // Loop through the section
    while (inSection) {
        y = y + yDelta; // Move to the next cell vertically
        x = x + xDelta; // Move to the next cell horizontally
        // Check if the cell is out of bounds
        if ((y >= (int)board.size()) || (x >= (int)board[0].size())) {
            inSection = false;
        // Check if the cell is blank
        } else if (board[y][x] == 0) {
            return false;
        // Check if the section has come to an end
        } else if ((board[y][x] <= 0) || (board[y][x] > 9)) {
            inSection = false;
        // If the cell is still within the section
        } else {
            total += board[y][x]; //Add the value of the cell to the total
        }
    }
    // Check if the total is equal to the sum if this section
    if (sum == total) {
        return true;
    }
    return false;
}

bool MainWindow::checkPuzzle(){
    // To store if a section is correct
    bool correct = true;
    // Loop through the board
    for (int y = 0; y < (int)board.size(); y++) {
        for (int x = 0; x < (int)board[0].size(); x++) {
            // Check for a sum cell
            if (board[y][x] >= 10) {
                // Get the vertical sum
                int vertSum = floor((int)(board[y][x]/10)%100);
                // If there is a vertical sum
                if (vertSum > 0) {
                    // Check if the vertical section is correct
                    if (!checkSection(vertSum, y, x, 1, 0)) {
                        // If not, move out of the loop
                        correct = false;
                        break;
                    }
                }
                // Get the horizontal sum
                int horizSum = floor(board[y][x]/1000);
                // If there is a horizontal sum
                if (horizSum > 0) {
                    // Check if the horizontal section is correct
                    if (!checkSection(horizSum, y, x, 0, 1)) {
                        // If not, move out of the loop
                        correct = false;
                        break;
                    }
                }
            }
        }
        // If a section doesn't add up
        if (!correct) {
            // Move out of the loop
            break;
        }
    }
    return correct;

}

/**
 * @brief MainWindow::puzzleSolved
 * This method is called when the puzzle is solved.
 * Changes the colour of the numbers to let the user know that the puzzle is solved.
 */
void MainWindow::puzzleSolved(){
    //sets the replay solution button to be enabled
    ui->replaySolutionButton->setEnabled(true);
    //loop through the board vector
    for(int i = 0; i<(int) board.size(); i++){
        for(int j = 0; j<(int)board[i].size(); j++){
            //change colours for cell containing 1-9 only
            if(board[i][j]>=1 && board[i][j]<=9){
                //new cell
                cell = new QStandardItem(QString::number(board[i][j]));
                f.setPointSize(30); //set the font size
                cell->setFont(f); //set the font
                // Set the color to dark green
                cell->setForeground(QColor::fromRgb(0,200,0));
                //text alignment
                cell->setTextAlignment(Qt::AlignCenter);
                //set the cell into the model
                model->setItem(i, j, cell);

            }
        }
    }


}

void MainWindow::on_RandomNumbers_clicked()
{
    //get text from the combo box
    QString size = ui->boardSize->currentText();
    //check if the user has acutally selected the size
    if (size != "Board Size"){
        //set up size of array
        int height = size.left(size.indexOf("x")).toInt(); //get the height of the board
        int width = size.right(size.indexOf("x")).toInt(); //get the width of the board
        setBoardSize(height, width); //initialise the board
        populateBoard(height, width);
        //create random layout
        //createRandomLayout();
        //populates the boardSolution vector
        //populateBoardRandom();
        //Adds up the horizontal and vertical sums
        //populateAnswerCells();

        //populates the board vector
        createBlankBoardFromSolution();
        //calculate solutions
        //calculateSolutions();
        //draws the board to the screen
        drawBoard();
    }

}

void MainWindow::on_saveFileButton_clicked()
{
    // Only fair to use
    saveBoard();
}

void MainWindow::on_loadFileButton_clicked()
{
    populateBoardFromFile();
}

void MainWindow::menuRequest(QPoint pos)
{
    if(showingSolution==true)
        return;

    // Retrieving the row and column of the mouse click on the grid
    QModelIndex index = ui->tableView->indexAt(pos);
    // Check whether index has both non negative x and y positions
    if (index.isValid()) {
        // If the cell is a blank cell (excludes sum cells, grey cells)
        if (board[index.row()][index.column()] >= 0 && board[index.row()][index.column()] <= 10) {
            // Create a menu item
            QMenu menu(this);
            // Set text for the action and add them to the menu
            menu.addAction("Clear set value");

            // Add actions to enter values: 1-9
            for (int i = 1; i <= 9; i++) {menu.addAction("Set value to " + QString::number(i));}

            // Action that was clicked on
            QAction *action = menu.exec(ui->tableView->viewport()->mapToGlobal(pos));

            // Making sure the user selects a QAction
            if (action) {
                // If it is a set action
                if (action->text().contains("Set value to ")) {

                    userMove newMove = userMove(index.row(), index.column(), board[index.row()][index.column()], action->text().right(1).toInt());
                    moves.push_back(newMove);
                    // Create a new cell with the selected number
                    cell = new QStandardItem(action->text().right(1));
                    // Update the board with the selected number
                    board[index.row()][index.column()] = action->text().right(1).toInt();
                    // Change the font size
                    f.setPointSize(30);
                    // Check if the puzzle is solved
                    if (checkPuzzle()){
                        // if puzzle is solved
                        puzzleSolved();
                    }
                // If it was clearValue action
                } else if (action->text().contains("Clear")) {

                    userMove newMove = userMove(index.row(), index.column(), board[index.row()][index.column()], 0);
                    moves.push_back(newMove);

                    // Create a cell with the default string for blank cell
                    cell = new QStandardItem(QString("1 2 3\n4 5 6\n7 8 9"));
                    // Update the cell in the board vector
                    board[index.row()][index.column()] = 0;
                    // Change the font size
                    f.setPointSize(9);
                }
                // Set font and alignment of the cell and set it to the model
                cell->setFont(f);
                cell->setTextAlignment(Qt::AlignCenter);
                model->setItem(index.row(), index.column(), cell);
            }
            // Now delete the action
            delete action;
        }
    }
}


/**
 * Returns a string in the format {x, y, old, new}
 * @brief MainWindow::movesToString
 * @return string of moves
 */
QString MainWindow::movesToString(){
    QString rString = "";
    for(auto i : moves) {
        rString += i.toString() + ",";
    }
    rString = rString.left(rString.length()-2);
    return rString + "\n";
}


void MainWindow::on_undoButton_clicked()
{
    undoMove();
}

void MainWindow::undoMove(){
    if (moves.size() != 0) {
        userMove m = moves.back();

        // Create a new cell with the selected number
        cell = new QStandardItem(m.getOldValue());
        // Update the board with the selected number
        board[m.getRow()][m.getColumn()] = m.getOldValue();
        // Change the font size
        f.setPointSize(30);

        cell->setFont(f);
        cell->setTextAlignment(Qt::AlignCenter);
        model->setItem(m.getRow(), m.getColumn(), cell);

        drawBoard();

        moves.pop_back();
    }
}

void MainWindow::on_replaySolutionButton_clicked()
{
    ui->replaySolutionButton->setEnabled(false);
    replaySolution();
}

void MainWindow::replaySolution(){
    showingSolution = true;
    createBlankBoardFromSolution();
    drawBoard();
    for(size_t i = 0; i<moves.size(); i++){
        // Create a new cell with the selected number
        cell = new QStandardItem(moves[i].getOldValue());
        // Update the board with the selected number
        board[moves[i].getRow()][moves[i].getColumn()] = moves[i].getNewValue();
        // Change the font size
        f.setPointSize(30);

        cell->setFont(f);
        cell->setTextAlignment(Qt::AlignCenter);
        model->setItem(moves[i].getRow(), moves[i].getColumn(), cell);

        drawBoard();

        //code below is from a question asked on stack overflow
        //http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
        //sleeps for 1 second
        QTime dieTime= QTime::currentTime().addSecs(1);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    }

    puzzleSolved();

    showingSolution = false;


}
