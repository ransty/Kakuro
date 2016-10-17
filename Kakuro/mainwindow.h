#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_resetButton_clicked();

    void on_generateButton_clicked();

    void on_playButton_clicked();

    void on_backButton_clicked();

    void on_quitButton_clicked();

private:
    Ui::MainWindow *ui;

    void generateKakuro();
};

#endif // MAINWINDOW_H
