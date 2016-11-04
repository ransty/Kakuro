/*
 * Software Development with C++ Assignment 2 - Kakuro
 * Group Members:
 * Oliver Norman - noroj001
 * Shyam Pindoria - pinsy004
 * Keano Porcaro - porkd002
 *
*/
#ifndef USERMOVE_H
#define USERMOVE_H
#include <string>
#include <QString>

class userMove
{
public:
    userMove();
    ~userMove();
    userMove(int, int, int, int);
    int getRow();
    int getColumn();
    int getOldValue();
    int getNewValue();
    QString toString();
private:
    int row;
    int column;
    int oldValue;
    int newValue;

};

#endif // USERMOVE_H
