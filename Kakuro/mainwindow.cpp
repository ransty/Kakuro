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

MainWindow::MainWindow(QWidget	*parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //srand used to create random numbers
    srand(time(NULL));
    ui->setupUi(this);
    QStandardItemModel *model = new QStandardItemModel(9, 8, this);
    ui->tableView->setModel(model);
    for(int row = 0; row < 13; row++) {
        for(int col = 0; col < 13; col++) {
            QStandardItem *value;
            QBrush brush = QBrush(QColor(Qt::black));
            QFont f("Smooth Sizes");
            // Generate random number
            int randNum = rand()%(15-1 + 1) + 1;
            f.setPointSize(30);
            // Place the random number inside the QStandardItem
            value = new QStandardItem(QString(QString::number(randNum)));
            // Determine wether we want that square to be a solid block
            // Based on either the input file or if we work it out, the randomly generated board (pt 8)
            if (row == 0 || col == 0 || randNum >= 10) {
                value->setBackground(brush);
                value->setText("");
                // This line makes the QStandardItem read-only
                value->setFlags(Qt::NoItemFlags);
            }
            value->setFont(f);
            value->setTextAlignment(Qt::AlignCenter);
            model->setItem(row, col, value);
        }
    }
}

MainWindow::~MainWindow()
{
   delete ui;
}
