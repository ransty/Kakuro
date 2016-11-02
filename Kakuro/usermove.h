#ifndef USERMOVE_H
#define USERMOVE_H
#include <string>

class userMove
{
public:
    userMove();
    ~userMove();
    userMove(int, int, int, int);
    int getX();
    int getY();
    int getOldValue();
    int getNewValue();
    std::string toString();
private:
    int x;
    int y;
    int oldValue;
    int newValue;

};

#endif // USERMOVE_H
