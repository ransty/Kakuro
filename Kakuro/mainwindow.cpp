#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_generateButton_clicked()
{
    // Generate button
    for (int i = 0; i <= ui->progressBar->maximum(); i++) {
        ui->progressBar->setValue(i);
    }

}

void MainWindow::on_resetButton_clicked()
{
    // Reset button
    ui->progressBar->setValue(0);
}
