#include "usermove.h"

userMove::userMove()
{

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


//this needs to output as so:
//"{x=5,y=4,o=1,n=6}"
std::string userMove::toString(){
    return "";
}
