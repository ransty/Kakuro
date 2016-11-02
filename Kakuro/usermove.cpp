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

userMove::userMove(int x, int y, int oldValue, int newValue){
    this->x = x;
    this->y = y;
    this->oldValue = oldValue;
    this->newValue = newValue;
}

int userMove::getNewValue(){
    return newValue;
}

int userMove::getOldValue(){
    return oldValue;
}

int userMove::getX(){
    return x;
}

int userMove::getY(){
    return y;
}

/**
 * @brief userMove::toString
 * @return {x, y, old, new}
 */
QString userMove::toString(){
    QString rString = "{x=" + QString::fromStdString(std::to_string(getX())) + ", y=" + QString::fromStdString(std::to_string(getY())) + ", o=" +
                                 QString::fromStdString(std::to_string(getOldValue())) + ", n=" + QString::fromStdString(std::to_string(getNewValue())) + "}";
    return rString;
}
