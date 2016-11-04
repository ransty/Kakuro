/*
 * Software Development with C++ Assignment 2 - Kakuro
 * Group Members:
 * Oliver Norman - noroj001
 * Shyam Pindoria - pinsy004
 * Keano Porcaro - porkd002
 *
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QPoint>
#include "usermove.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_saveFileButton_clicked();
    void on_loadFileButton_clicked();
    void menuRequest(QPoint pos);

    void on_undoButton_clicked();

    void on_replaySolutionButton_clicked();

    void on_redoButton_clicked();

    void on_boardSize_currentIndexChanged(int index);

    void on_generateBoardButton_clicked();

    void on_clueButton_clicked();

    void on_solvePuzzleButton_clicked();

private:
    void printMyMoves();
    Ui::MainWindow *ui;
    void drawBoard();
    void setBoardSize(int heigth, int width);
    void populateBoard(int heigth, int width);
    void populateBoardRandom();
    void populateAnswerCells();
    void createRandomLayout();
    void populateBoardFromFile();
    void saveBoard();
    bool validateFile(QFile*);
    void createBlankBoardFromSolution();
    bool checkSection(int sum, int y, int x, int yDelta, int xDelta);
    bool checkPuzzle();
    void changeAnswerCellColor(QColor);
    void valuesInSection(int sum, int y, int x, int yDelta, int xDelta);
    void calculatePossibleValues();
    void removeInvalidValues(int sum, int y, int x, int yDelta, int xDelta, std::vector<int> numsPresent);
    void undoMove();
    void redoMove();
    void replaySolution();
    void checkButtons();
    std::vector<std::vector<double>> board;
    std::vector<std::vector<double>> boardSolution;
    std::vector<userMove> moves;
    std::vector<userMove> undoMoves;
    QString movesToString();
    bool showingSolution;
    bool solvingSolution;
    bool stoppedReplay;

};

#endif // MAINWINDOW_H
