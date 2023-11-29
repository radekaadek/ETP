// kolko i krzyzyk board

#include <iostream>
#include <string>

class Board {
private:
    char board[3][3];
    char player;
    char winner;
    char checkWinner(int currX, int currY) const;
public:
    bool checkMove(int x, int y) const;
    Board();
    void print() const;
    void move(int x, int y);
    char getWinner() const;
    char getPlayer() const;
    void reset();
};
