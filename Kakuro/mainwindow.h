#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

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

private:
    Ui::MainWindow *ui;
    void drawBoard();
    void setBoardSize(int, int);
    void populateBoardRandom();
    void populateBoardFromFile();
    bool validateFile(QFile*);
    std::vector<std::vector<double>> board;
};

#endif // MAINWINDOW_H
