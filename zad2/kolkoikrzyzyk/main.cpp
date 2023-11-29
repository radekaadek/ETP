#include <serial.h>
#include <board.h>
#include <iostream>
#include <thread>
#include <chrono>

void readSerial(Serial& serial) {
    // Get buffer size
    DWORD queueSize = serial.queueSize();
    while (true) {
        // try to read the buffer
        queueSize = serial.queueSize();
        if (queueSize > 0) {
            std::string data = serial.ReadData(queueSize);
            std::cout << data;
        }
        else {
            // wait for 200ms
            thSleep(200);
        }
    }
}

std::string getPort(int argc, char* argv[]) {
    // scan argv for -port
    std::string port = "COM6";
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-port") {
            if (i + 1 <= argc) {
                return argv[i + 1];
            }
            else {
                std::cerr << "No port specified\n";
                return port;
            }
        }
    }
    return port;
}


int main(int argc, char* argv[]) {
    const std::string lineEnd = "\r\n";

    std::string port = getPort(argc, argv);

    // Instantiate the Serial object
    Serial serial(port);
    // Check if the connection was successfull
    if (serial.IsConnected()) {
        std::cout << "The port is connected\n";
    }
    else {
        std::cout << "Failed to connect to the port\n";
        return 1;
    }

    // Instantiate the Board object
    char myPlayer = 'O';
    Board board;
    std::string move;
    while(board.getWinner() == ' ') {
        std::cout << "\n";
        board.print();
        std::cout << "Player " << board.getPlayer() << " move: ";
        if (board.getPlayer() == myPlayer) {
            std::cin >> move;
            // check if the move is valid
            while (move.size() != 2 ||
                    board.checkMove(move[0] - '0', move[1] - '0') == false) {
                std::cout << "Invalid move on the board \"" << move << "\", try again: ";
                std::cin >> move;
            }
            serial.WriteData(move + lineEnd);
        }
        else {
            move = serial.ReadLine(lineEnd);
            while (move.size() != 2+lineEnd.size() ||
                    board.checkMove(move[0] - '0', move[1] - '0') == false) {
                std::cout << "\nInvalid move on the board \"" << move.substr(0, move.length()-lineEnd.length()) << "\", try again: ";
                move = serial.ReadLine(lineEnd);
            }
            std::cout << move << "\n";
        }
        board.move(move[0] - '0', move[1] - '0');
    }
    board.print();
    std::cout << "The winner is " << board.getWinner() << "!\n";

    return 0;
}