#include <board.h>

Board::Board() {
    reset();
}

void Board::print() const {
    std::cout << "  0 1 2\n";
    for (int i = 0; i < 3; i++) {
        std::cout << i << " ";
        for (int j = 0; j < 3; j++) {
            std::cout << board[i][j] << " ";
        }
        std::cout << "\n";
    }
}

char Board::checkWinner(int currX, int currY) const {
    // check current row
    if (board[currX][0] == board[currX][1] && board[currX][1] == board[currX][2]) {
        return board[currX][0];
    }
    // check current column
    if (board[0][currY] == board[1][currY] && board[1][currY] == board[2][currY]) {
        return board[0][currY];
    }
    // check diagonal
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return board[0][0];
    }
    // check anti-diagonal
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return board[0][2];
    }
    return ' ';
}

bool Board::checkMove(int x, int y) const {
    if (x < 0 || x > 2 || y < 0 || y > 2) {
        return false;
    }
    if (board[x][y] != ' ') {
        return false;
    }
    return true;
}

void Board::move(int x, int y) {
    if (!checkMove(x, y)) {
        return;
    }
    if (board[x][y] == ' ') {
        board[x][y] = player;
        if (player == 'X') {
            player = 'O';
        }
        else {
            player = 'X';
        }
    }
    this->winner = checkWinner(x, y);
}

char Board::getWinner() const {
    return winner;
}

char Board::getPlayer() const {
    return player;
}

void Board::reset() {
    player = 'X';
    winner = ' ';
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

