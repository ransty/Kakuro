#include "usermove.h"
#include <QString>

userMove::userMove()
{

}

/**
 * Destructor
 * @brief userMove::~userMove
 */
userMove::~userMove() {
    // nothing to delete, no pointers
}

userMove::userMove(int row, int column, int oldValue, int newValue){
    this->row = row;
    this->column = column;
    this->oldValue = oldValue;
    this->newValue = newValue;
}

int userMove::getNewValue(){
    return newValue;
}

int userMove::getOldValue(){
    return oldValue;
}

int userMove::getRow(){
    return row;
}

int userMove::getColumn(){
    return column;
}

/**
 * @brief userMove::toString
 * @return {x, y, old, new}
 */
QString userMove::toString(){
    QString rString = "{" + QString::fromStdString(std::to_string(getRow())) + "," + QString::fromStdString(std::to_string(getColumn())) + "," +
                                 QString::fromStdString(std::to_string(getOldValue())) + "," + QString::fromStdString(std::to_string(getNewValue())) + "}";
    return rString;
}
