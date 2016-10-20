#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFont>
#include <string>
#include <QBrush>
#include <QColor>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <sstream>

//board to parse to drawBoard()
std::vector<std::vector<double>> board;

MainWindow::MainWindow(QWidget	*parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //srand used to create random numbers
    srand(time(NULL));

    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
   delete ui;
}

//uses the board vector to draw the board to the screen
void MainWindow::drawBoard(){

    //the board layout
    QStandardItemModel *model = new QStandardItemModel(board.size(), board[0].size(), this);

    ui->tableView->setModel(model);

    //loops through each cell in the board array
    for(int i = 0; i<(int) board.size(); i++){
        for(int j = 0; j<(int)board[i].size(); j++){


            //creates a new cell
            QStandardItem *cell;
            //checks if the current board value is 0
            //if a value is 0, it is a black square
            if(board[i][j] == 0){
                //sets all formatting for a black cell
                cell = new QStandardItem();
                QBrush brush = QBrush(QColor(Qt::black));
                cell->setBackground(brush);
                cell->setFlags(Qt::NoItemFlags);

            }
            else{
                //gives the cell a number from the board vector
                cell = new QStandardItem(QString::number(board[i][j]));
            }

            //a bit of formatting for the cell
            QFont f("Smooth Sizes");
            f.setPointSize(30);
            cell->setFont(f);
            cell->setTextAlignment(Qt::AlignCenter);


            //sets the cell into the model
            model->setItem(i, j, cell);

        }
    }

}

//sets the board size using two parameters
void MainWindow::setBoardSize(int height, int width){
    //pretty simple,
    //just sets the size of the multidimentional array
    board.resize(height);
    for(int i = 0; i<height; i++){
        board[i].resize(width);
    }
}

//populates the board with random numbers
void MainWindow::populateBoardRandom(){

    //loops through each rown
    for(int i = 0; i<(int)board.size(); i++){
        //loops through each column in each row
        for(int j = 0; j<(int)board[i].size(); j++){
            //first row and column
            if(i == 0 || j == 0){
                board[i][j] = 0;
            }
            else{
                //generate random number between 1 and 9
                int randNum = rand()%(9-1 + 1) + 1;
                //places the random number in the current board cell
                board[i][j] = randNum;
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
           lineLength = line.length();
       }
       else{
           //line is not the same length as the last line
           if(lineLength!=line.length()){
               //failed
               return false;
           }
       }


       //checks if the current line contains anything but numbers and commas
       QRegExp r("^[0-9](,[0-9])*$");
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
    //populates the board vector
    populateBoardRandom();
    //draws the board to the screen
    drawBoard();

}

void MainWindow::on_saveFileButton_clicked()
{

}

void MainWindow::on_loadFileButton_clicked()
{
    populateBoardFromFile();
    drawBoard();
}
