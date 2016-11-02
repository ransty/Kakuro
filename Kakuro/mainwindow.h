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
    void on_RandomNumbers_clicked();
    void on_saveFileButton_clicked();
    void on_loadFileButton_clicked();
    void menuRequest(QPoint pos);

    void on_undoButton_clicked();

    void on_replaySolutionButton_clicked();

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
    void puzzleSolved();
    void undoMove();
    void replaySolution();
    std::vector<std::vector<double>> board;
    std::vector<std::vector<double>> boardSolution;
    std::vector<userMove> moves;
    QString movesToString();
    bool showingSolution;

};

#endif // MAINWINDOW_H
