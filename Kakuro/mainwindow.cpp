#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Show the start widget initially
    ui->stackedWidget->setCurrentWidget(ui->start);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Generate button
 */
void MainWindow::on_generateButton_clicked()
{
    for (int i = 0; i <= ui->progressBar->maximum(); i++) {
        ui->progressBar->setValue(i);
    }

    // Generate a kakuro puzzle
    generateKakuro();

    // Enable the play button
    ui->playButton->setEnabled(true);

    // Disable the generate button
    ui->generateButton->setEnabled(false);

    // Enable the reset button
    ui->resetButton->setEnabled(true);

}

/**
 * Reset button
 */
void MainWindow::on_resetButton_clicked()
{
    // Reset the progress bar
    ui->progressBar->setValue(0);

    // Disable the play button
    ui->playButton->setEnabled(false);

    // Enable the generate button
    ui->generateButton->setEnabled(true);

    // Disable the reset button
    ui->resetButton->setEnabled(false);
}

/**
 * Quit button
 */
void MainWindow::on_quitButton_clicked()
{
    this->close();
}

/**
 * Play button
 */
void MainWindow::on_playButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->kakuro);
}

/**
 * Back button
 */
void MainWindow::on_backButton_clicked()
{
    // Reset the game
    on_resetButton_clicked();

    // Show the start screen
    ui->stackedWidget->setCurrentWidget(ui->start);
}

/**
 * Method to generate a kakuro puzzle
 */
void MainWindow::generateKakuro()
{

}
