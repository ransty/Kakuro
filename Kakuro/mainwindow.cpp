/*
 * Software Development with C++ Assignment 2 - Kakuro
 * Group Members:
 * Oliver Norman - noroj001
 * Shyam Pindoria - pinsy004
 * Keano Porcaro - porkd002
 *
*/
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
#include <unistd.h>
#include <QStyle>
#include <QDesktopWidget>
#include <QSysInfo>




//both boards use the same number standards:

//-1 = answer cell - Initially,
//after calculations of sums, it is stored in the following format
//horizontal sums are multiplied by 1000 and vertical sums are multiplied by 10 both values are added if in the same cell
//E.g.  5000 will be horizontal sum 5
//      160 will be vertical sum 16
// if in the same cell, it will be 5160
//Another E.g. 20220 means 20 - horizontal and 22 - vertical
//See MainWindow::populateAnswerCell() to see how this works

//-2 = black cell

//0 = blank cell

//1-9 = filled in cell


//board holds the current values of the visual game board
std::vector<std::vector<double>> board;

//boardSolution contains the numbers that are needed to solve the board
//these numbers are never visible to the user
std::vector<std::vector<double>> boardSolution;

std::vector<userMove> moves;

std::vector<userMove> undoMoves;

//the board layout
QStandardItemModel *model;
//creates a new cell
QStandardItem *cell;
QFont f("Smooth Sizes");

MainWindow::MainWindow(QWidget	*parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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
    ui->boardSize->addItem("15x30");

    showingSolution = false;
    solvingSolution = false;
    stoppedReplay = false;
    checkButtons();
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
                QBrush brush = QBrush(QColor(Qt::gray));
                cell->setBackground(brush);
                cell->setForeground(QBrush(QColor(Qt::white)));
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

                f.setPointSize(6); // sets font size
                if (QSysInfo::productType() == "osx"){
                    f.setPixelSize(9);
                }
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
            // Black cell
            else if(board[i][j] <= -2){
                //sets all formatting for a black cell
                cell = new QStandardItem();
                QBrush brush = QBrush(QColor(Qt::black));
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


        }

    }
    checkButtons();
    calculatePossibleValues();
    int windowHeight = board.size() * 47; //get the height of the board
    int windowWidth = board[0].size() * 41; //get the width of the board

    // Sizes should not go below minimum
    windowWidth = windowWidth < 890 ? 890 : windowWidth;
    windowHeight = windowHeight < 265 ? 265 : windowHeight;
    // Resize the window
    this->window()->resize(windowWidth, windowHeight);
    // Set the window to the center of the screen
    // So that the window does not go out of the screen
    // The piece of code below was snipped from a Qt Wiki post "How to Center a Window on the Screen"
    // https://wiki.qt.io/How_to_Center_a_Window_on_the_Screen
    this->window()->setGeometry(
                QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    this->window()->size(),
                    qApp->desktop()->availableGeometry()
                    )
                );

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
    for(int i = 0; i<height; i++){
        board[i].resize(width);
        boardSolution[i].resize(width);
    }
}

void MainWindow::populateBoard(int height, int width){
    moves.clear();
    undoMoves.clear();
    checkButtons();

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
    else if (height == 15 && width == 30) {
        boardSolution = {
            {-2,-2,60,170,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,60,100,-2,300,40,-2,-2,160,100,-2,240,160,-2,-2,-2,-2},
            {-2,10170,2,8,30,-2,-2,-2,-2,160,240,60,-2,7420,3,4,11000,8,3,30,11000,9,2,17000,8,9,-2,-2,-2,-2},
            {20000,8,1,9,2,170,-2,-2,20040,9,8,3,14000,8,1,5,9160,6,1,2,10340,7,3,16170,9,7,-2,-2,-2,-2},
            {12000,9,3,9000,1,8,230,19030,1,7,9,2,22300,7,2,1,3,9,10000,1,9,16040,1,8,7,-2,-2,-2,-2,-2},
            {-2,-2,-2,-2,20000,9,6,2,3,19000,7,1,6,5,-2,13160,6,7,-2,24300,8,3,4,9,-2,160,110,60,-2,-2},
            {-2,-2,-2,-2,-2,10040,9,1,170,60,-2,15000,9,6,9000,7,2,-2,14000,6,7,1,-2,40,12160,7,2,3,-2,-2},
            {-2,-2,-2,-2,9250,1,8,11000,8,3,-2,17290,8,9,10000,9,1,290,12000,8,4,240,24000,1,7,9,5,2,-2,-2},
            {-2,-2,-2,8230,5,3,40,10160,9,1,15340,5,7,3,150,5000,4,1,23300,9,6,8,12040,3,9,4030,3,1,-2,-2},
            {-2,-2,12000,9,3,10030,1,9,15000,2,4,9,5000,4,1,160,19000,4,8,7,8000,7,1,-2,3230,2,1,-2,-2,-2},
            {-2,-2,25000,6,8,1,3,7,-2,16030,9,7,-2,12000,3,9,10000,3,7,-2,12000,9,3,9030,8,1,-2,-2,-2,-2},
            {-2,-2,19000,8,9,2,-2,40,15120,1,6,8,-2,11290,4,7,17000,8,9,70,240,-2,8170,2,6,30,-2,-2,-2,-2},
            {-2,-2,-2,-2,-2,-2,15230,1,5,2,7,170,13000,8,5,240,18070,2,6,1,9,20040,8,1,9,2,160,-2,230,40},
            {-2,-2,-2,-2,-2,13030,8,3,2,17170,8,9,26030,5,2,9,4,6,20000,2,8,1,9,-2,8000,1,7,10040,9,1},
            {-2,-2,-2,-2,8000,2,6,12000,4,8,16000,8,1,7,13000,7,1,5,14000,4,7,3,-2,-2,-2,21000,9,1,8,3},
            {-2,-2,-2,-2,10000,1,9,10000,1,9,-2,11000,2,9,10000,8,2,-2,-2,-2,-2,-2,-2,-2,-2,-2,9000,3,6,-2} };
    }
}


void MainWindow::createBlankBoardFromSolution(){
    // this is where we copy all the black cells and answer cells from boardSolution to board

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
    checkButtons();
}


//populates the board from a file
void MainWindow::populateBoardFromFile(){


    //opens a dialog box for the user to select a file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    
    
    // Loads the file
    QFile file(fileName);



    file.open(QIODevice::ReadWrite);

    if(!validateFile(&file)){
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

        //sets the file back to the first character
        //as this will change during validation
        file.seek(0);

        QTextStream in(&file);


        //reads the whole file into a stringstream
        std::stringstream ss(in.readAll().toStdString());
        std::string token;

        //strings to hold each part of the file
        std::string sizeString;
        std::string undoMovesString;
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
            else if (loopNumber == 3){
                movesString = token;
            } else {
                undoMovesString = token;
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
            int bracketLoc = moveToken.find('}');
            if(bracketLoc != -1){
                //gets the values in the format 1,1,1,1
                moveToken = moveToken.substr(0, bracketLoc);


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

        //////////////////////////////
        //start sorting of undomoves//
        //////////////////////////////

        //need to clear the current move board
        undoMoves.clear();

        //does our normal looping through a string and stopping at each ,
        std::stringstream undoMovesS(undoMovesString);
        std::string undoToken;

        //need this test to remove whitespace line
        while (getline(undoMovesS,undoToken, '{'))
        {
            //checks if the line is valid
            int bracketLoc = undoToken.find('}');
            if(bracketLoc != -1){
                //gets the values in the format 1,1,1,1
                undoToken = undoToken.substr(0, bracketLoc);


                //variables to store the data in
                int rowTemp;
                int columnTemp;
                int oldTemp;
                int newTemp;

                //counter to know which value it is up to
                int count = 0;

                //splits the string into individual integers
                std::stringstream boardSs2(undoToken);
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
                undoMoves.push_back(myMove);
            }

        }


        /////////////////////////////////
        //finished sorting of undomoves//
        /////////////////////////////////

        //undoMoves.clear();
        drawBoard();
        //checks if puzzle solved
        checkPuzzle();
        calculatePossibleValues();
    }


    checkButtons();
}

//validates if a file is okay to be used
bool MainWindow::validateFile(QFile* file){
    //safety check for file
    qDebug() << file->isReadable();
    //returns ƒalse if not readable
    if(!file->isReadable()){
        return false;
    }

    //check for 5 sections
    //size, board, boardSolution, undo, redo
    QTextStream fileStream(file);
    int starCount = 0;

    while (!fileStream.atEnd()) {
        //reads the next line
        QString line = fileStream.readLine();
        if(line.contains("*")){
            starCount++;
        }
    }
    if(starCount!=4)
        return false;


    QTextStream in(file);




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

        out << "\n*\n";

        // Now print the undoMoves
        for (auto i : undoMoves) {
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

    if(board.size()==0)
        correct = false;

    //sets the color of the board numbers

    if(correct)
        changeAnswerCellColor(QColor::fromRgb(0,200,0));
    else
        changeAnswerCellColor(QColor::fromRgb(0,0,0));


    return correct;

}

/**
 * @brief MainWindow::changeAnswerCellColor
 * This method is called when the puzzle is solved.
 * Changes the colour of the numbers to let the user know that the puzzle is solved.
 */
void MainWindow::changeAnswerCellColor(QColor c){
    //loop through the board vector
    for(int i = 0; i<(int) board.size(); i++){
        for(int j = 0; j<(int)board[i].size(); j++){
            //change colours for cell containing 1-9 only
            if(board[i][j]>=1 && board[i][j]<=9){
                model->item(i,j)->setForeground(c);
            }
        }
    }
}


/**
 * @brief MainWindow::valuesInSection figures out what values are present in the section and calls removeInvalid Values
 * @param sum sum of the current section
 * @param y index of the cell
 * @param x index of the cell
 * @param yDelta change in y
 * @param xDelta change in x
 */
void MainWindow::valuesInSection(int sum, int y, int x, int yDelta, int xDelta){

    // initial x and y
    int originy = y;
    int originx = x;
    std::vector <int> numsPresent = {};
    // True if board[y][x] is in the current section
    bool inSection = true;
    // Loop through the section
    while (inSection) {
        y = y + yDelta; // Move to the next cell vertically
        x = x + xDelta; // Move to the next cell horizontally
        // Check if the cell is out of bounds
        if ((y >= (int)board.size()) || (x >= (int)board[0].size())) {
            inSection = false;
            // Check if the section has come to an end
        } else if ((board[y][x] < 0) || (board[y][x] > 9)) {
            inSection = false;
            // push 1-9 values to vector numsPresent
        } else if (board[y][x] != 0) {
            numsPresent.push_back(board[y][x]);
        }
    }
    // Display values that are not in the grid
    removeInvalidValues(sum, originy, originx, yDelta, xDelta, numsPresent);
}

/**
 * @brief MainWindow::calculatePossibleValues
 * Loop through the board and search for sections, call valuesInSection(...) for each section
 */
void MainWindow::calculatePossibleValues(){
    // Loop through the board and reset it back to normal
    // This done because checking is done based on the numbers currently displaying
    // If they are not reset, then when the user clears a number, it will display incorrect values
    for (int y = 0; y < (int)board.size(); y++) {
        for (int x = 0; x < (int)board[0].size(); x++) {
            // Reset empty spaces back to normal
            if (board[y][x] == 0){
                model->item(y, x)->setText(QString("1 2 3\n4 5 6\n7 8 9"));
                model->item(y, x)->setBackground(QBrush(QColor(Qt::white)));
            }
        }
    }
    // Loop through the board
    for (int y = 0; y < (int)board.size(); y++) {
        for (int x = 0; x < (int)board[0].size(); x++) {
            // Check for a sum cell
            if (board[y][x] >= 10) {
                // Get the vertical sum
                int vertSum = floor((int)(board[y][x]/10)%100);
                // If there is a vertical sum
                if (vertSum > 0) {
                    valuesInSection(vertSum, y, x, 1, 0);
                }
                // Get the horizontal sum
                int horizSum = floor(board[y][x]/1000);
                // If there is a horizontal sum
                if (horizSum > 0) {
                    valuesInSection(horizSum, y, x, 0, 1);
                }
            }
        }
    }
}

void MainWindow::removeInvalidValues(int sum, int y, int x, int yDelta, int xDelta, std::vector<int> numsPresent){

    bool inSection = true;
    // Loop through the section
    while (inSection) {
        y = y + yDelta; // Move to the next cell vertically
        x = x + xDelta; // Move to the next cell horizontally
        // Check if the cell is out of bounds
        if ((y >= (int)board.size()) || (x >= (int)board[0].size())) {
            inSection = false;
            // Check if the cell is blank and set the possible values
        } else if (board[y][x] == 0) {
            // Get the values that are currently displaying
            QString strNums = model->item(y, x)->text();
            std::vector<int> nums; // Vector to store the values currently displayed
            nums.clear();
            // To convert the string to integers
            std::istringstream stream(strNums.toStdString());
            int tmp; //Temporary storage for a number in the string
            // Checks if the character can be parsed into an integer and pushes only those
            while (stream >> tmp) {
                nums.push_back(tmp);
            }
            // Loop through the numbers
            for (int i = 0; i < (int)nums.size(); i++){
                // if the number is a sum, set it to 0 (because 0 will be ignored later)
                if (nums[i] >= sum){
                    nums[i] = 0;
                }
                // Loop through numsPresent and check if a numeber matches with i (current number)
                // Set it to 0 if it does
                for (int num : numsPresent){
                    if (nums[i] == num){
                        nums[i] = 0;
                    }
                }
            }
            // Clear the string to make it ready to store values
            strNums = "";
            // Count of numbers appended to the string
            int count = 1;
            for (int i = 0; i < (int)nums.size(); i++){
                if (nums[i] != 0){
                    // Concatenate non 0 values to the string
                    strNums = strNums + QString::fromStdString(std::to_string(nums[i]));
                    // For adding spaces appropriately
                    if (i+1 != 9){
                        if((count)%3 == 0){
                            strNums += '\n';
                        } else {
                            strNums += ' ';
                        }
                    }
                    count++;
                }
            }
            int values = 0;
            for (int num : nums){
                if (num != 0){
                    values++;
                }
            }
            // If a given cell has only one possible value
            if (values == 1){
                // Set the color to yellow
                model->item(y, x)->setBackground(QColor::fromRgb(255,255,130));
            // If a cell has no possible value
            } else if (values == 0) {
                // Set the color to red
                model->item(y, x)->setBackground(QColor::fromRgb(255,130,130));
            }
            model->item(y, x)->setText(strNums);
          // Check if the section has come to an end
        } else if ((board[y][x] < 0) || (board[y][x] > 9)) {
            inSection = false;
        }
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
        // If the cell is a blank cell (excludes sum cells, black cells)
        if (board[index.row()][index.column()] >= 0 && board[index.row()][index.column()] <= 10) {
            // Create a menu item
            QMenu menu(this);
            // Set text for the action and add them to the menu
            menu.addAction("Clear set value");

            // Add actions to enter values: 1-9
            for (int i = 1; i <= 9; i++) {
                //uncomment this line to only show possible choices from the menu
                //if(model->item(index.row(), index.column())->text().contains(QString::number(i)))
                    menu.addAction("Set value to " + QString::number(i));
            }

            // Action that was clicked on
            QAction *action = menu.exec(ui->tableView->viewport()->mapToGlobal(pos));

            // Making sure the user selects a QAction
            if (action) {
                // If it is a set action
                if (action->text().contains("Set value to ")) {

                    userMove newMove = userMove(index.row(), index.column(), board[index.row()][index.column()], action->text().right(1).toInt());
                    moves.push_back(newMove);
                    //all undos should be cleared
                    undoMoves.clear();

                    // Create a new cell with the selected number
                    cell = new QStandardItem(action->text().right(1));
                    // Update the board with the selected number
                    board[index.row()][index.column()] = action->text().right(1).toInt();
                    // Change the font size
                    f.setPointSize(30);

                    // If it was clearValue action
                } else if (action->text().contains("Clear")) {

                    userMove newMove = userMove(index.row(), index.column(), board[index.row()][index.column()], 0);
                    moves.push_back(newMove);
                    //all undos should be cleared
                    undoMoves.clear();

                    // Create a cell with the default string for blank cell
                    cell = new QStandardItem(QString("1 2 3\n4 5 6\n7 8 9"));
                    // Update the cell in the board vector
                    board[index.row()][index.column()] = 0;
                    // Change the font size
                    f.setPointSize(6); // sets font size
                    if (QSysInfo::productType() == "osx"){
                        f.setPixelSize(9);
                    }
                }
                // Set font and alignment of the cell and set it to the model
                cell->setFont(f);
                cell->setTextAlignment(Qt::AlignCenter);
                model->setItem(index.row(), index.column(), cell);

                // Check if the puzzle is solved


                calculatePossibleValues();
            }
            // Now delete the action
            delete action;
        }
    }
    checkPuzzle();
    checkButtons();
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

        undoMoves.push_back(moves.back());
        moves.pop_back();

    }
    checkButtons();
    calculatePossibleValues();
}

void MainWindow::redoMove(){
    if(undoMoves.size() != 0){
        userMove m = undoMoves.back();

        // Create a new cell with the selected number
        cell = new QStandardItem(m.getOldValue());
        // Update the board with the selected number
        board[m.getRow()][m.getColumn()] = m.getNewValue();
        // Change the font size
        f.setPointSize(30);

        cell->setFont(f);
        cell->setTextAlignment(Qt::AlignCenter);
        model->setItem(m.getRow(), m.getColumn(), cell);

        drawBoard();

        moves.push_back(undoMoves.back());
        undoMoves.pop_back();


    }
    checkButtons();
    //checks if puzzle is solved
    checkPuzzle();

    calculatePossibleValues();

}

void MainWindow::on_replaySolutionButton_clicked()
{
    if(showingSolution){
        stoppedReplay = true;
    }
    qDebug() <<stoppedReplay;
    if(!stoppedReplay){
        ui->replaySolutionButton->setText("STOP");
        replaySolution();
    }
    else{
        stoppedReplay = true;
        ui->replaySolutionButton->setText("Replay Solution");

    }
    checkButtons();

}

void MainWindow::replaySolution(){
    qDebug() <<"here";
    showingSolution = true;
    createBlankBoardFromSolution();
    //drawBoard();
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
        calculatePossibleValues();
        checkPuzzle();
        //code below is from a question asked on stack overflow
        //http://stackoverflow.com/questions/3752742/how-do-i-create-a-pause-wait-function-using-qt
        //sleeps for 1 second
        if(stoppedReplay == false){
            QTime dieTime= QTime::currentTime().addSecs(1);
            while (QTime::currentTime() < dieTime)
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }


    }

    ui->replaySolutionButton->setText("Replay Solution");
    stoppedReplay = false;

    checkPuzzle();

    showingSolution = false;
    checkButtons();


}

void MainWindow::on_redoButton_clicked()
{
    redoMove();
}

void MainWindow::checkButtons(){
    ui->undoButton->setEnabled(moves.size() != 0 && !showingSolution && !solvingSolution);
    ui->redoButton->setEnabled(undoMoves.size() != 0 && !solvingSolution);
    ui->replaySolutionButton->setEnabled((checkPuzzle() && !solvingSolution) || showingSolution);
    ui->saveFileButton->setEnabled(board.size() != 0 && boardSolution.size() != 0 && !showingSolution && !solvingSolution);
    ui->loadFileButton->setEnabled(!showingSolution && !solvingSolution);
    ui->generateBoardButton->setEnabled(!showingSolution && ui->boardSize->currentIndex() != 0 && !solvingSolution);
    ui->clueButton->setEnabled(!showingSolution && !checkPuzzle() && board.size() != 0 && !solvingSolution);
    ui->solvePuzzleButton->setEnabled(!solvingSolution && !checkPuzzle() && !showingSolution && board.size() != 0);
}

void MainWindow::on_boardSize_currentIndexChanged(int index)
{
    checkButtons();
}

void MainWindow::on_generateBoardButton_clicked()
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

        //populates the board vector
        createBlankBoardFromSolution();
        //calculate solutions
        //calculateSolutions();
        //draws the board to the screen
        drawBoard();

        ui->boardSize->setCurrentIndex(0);

    }
    checkButtons();
}

void MainWindow::on_clueButton_clicked()
{
    ui->clueButton->setEnabled(false);

    srand(time(0));

    std::vector<std::vector<int>> tempStorage;

    for(int i = 0; i<(int) board.size(); i++){
        for(int j = 0; j<(int)board[i].size(); j++){
            if(board[i][j] == 0){
                std::vector<int> a = {i,j};
                tempStorage.push_back(a);
            }
        }
    }


    if(tempStorage.size()!=0){

        int tempInt = (rand() % tempStorage.size() + 0);

        userMove newMove = userMove(tempStorage[tempInt][0], tempStorage[tempInt][1], board[tempStorage[tempInt][0]][tempStorage[tempInt][1]], boardSolution[tempStorage[tempInt][0]][tempStorage[tempInt][1]]);
        moves.push_back(newMove);
        //all undos should be cleared
        undoMoves.clear();

        board[tempStorage[tempInt][0]][tempStorage[tempInt][1]] = boardSolution[tempStorage[tempInt][0]][tempStorage[tempInt][1]];
    }

    drawBoard();
    checkButtons();

}

void MainWindow::on_solvePuzzleButton_clicked()
{
    solvingSolution = true;
    checkButtons();


    //removes values that aren't correct
    for(int i = 0; i<(int) board.size(); i++){
        for(int j = 0; j<(int)board[i].size(); j++){
            if(board[i][j] != boardSolution[i][j]){
                undoMoves.clear();
                userMove newMove = userMove(i, j, board[i][j], 0);
                board[i][j] = 0;

            }
        }
    }

    for(int i = 0; i<(int) board.size(); i++){
        for(int j = 0; j<(int)board[i].size(); j++){

            if(board[i][j] == 0){
                undoMoves.clear();
                userMove newMove = userMove(i, j, board[i][j], boardSolution[i][j]);
                board[i][j] = boardSolution[i][j];

                moves.push_back(newMove);
                //all undos should be cleared


                drawBoard();
                //sleeps for 1 second
                QTime dieTime= QTime::currentTime().addMSecs(250);
                while (QTime::currentTime() < dieTime)
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }



        }
    }
    solvingSolution = false;
    checkButtons();
}
