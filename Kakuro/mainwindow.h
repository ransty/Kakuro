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

private:
    void printMyMoves();
    Ui::MainWindow *ui;
    void drawBoard();
    void setBoardSize(int, int);
    void populateBoardRandom();
    void populateAnswerCells();
    void saveBoard();
    void createRandomLayout();
    void populateBoardFromFile();
    void createBlankBoardFromSolution();
    bool validateFile(QFile*);
    std::vector<std::vector<double>> board;
    std::vector<std::vector<double>> boardSolution;
    std::vector<userMove> moves;
    QString movesToString();
};

#endif // MAINWINDOW_H
