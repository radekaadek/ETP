// #include <serial.h>
// #include <board.h>
// #include <iostream>
// #include <thread>
// #include <chrono>

// void readSerial(Serial& serial) {
//     // Get buffer size
//     DWORD queueSize = serial.queueSize();
//     while (true) {
//         // try to read the buffer
//         queueSize = serial.queueSize();
//         if (queueSize > 0) {
//             std::string data = serial.ReadData(queueSize);
//             std::cout << data;
//         }
//         else {
//             // wait for 200ms
//             thSleep(200);
//         }
//     }
// }

// std::string getPort(int argc, char* argv[]) {
//     // scan argv for -port
//     std::string port = "COM6";
//     for (int i = 1; i < argc; i++) {
//         if (std::string(argv[i]) == "-port") {
//             if (i + 1 <= argc) {
//                 return argv[i + 1];
//             }
//             else {
//                 std::cerr << "No port specified\n";
//                 return port;
//             }
//         }
//     }
//     return port;
// }

// int main(int argc, char* argv[]) {
//     const std::string lineEnd = "\r\n";

//     std::string port = getPort(argc, argv);

//     // Instantiate the Serial object
//     Serial serial(port);
//     // Check if the connection was successfull
//     if (serial.IsConnected()) {
//         std::cout << "The port is connected\n";
//     }
//     else {
//         std::cout << "Failed to connect to the port\n";
//         return 1;
//     }

//     // Instantiate the Board object
//     char myPlayer = 'O';
//     Board board;
//     std::string move;
//     while(board.getWinner() == ' ') {
//         std::cout << "\n";
//         board.print();
//         std::cout << "Player " << board.getPlayer() << " move: ";
//         if (board.getPlayer() == myPlayer) {
//             std::cin >> move;
//             // check if the move is valid
//             while (move.size() != 2 ||
//                     board.checkMove(move[0] - '0', move[1] - '0') == false) {
//                 std::cout << "Invalid move on the board \"" << move << "\", try again: ";
//                 std::cin >> move;
//             }
//             serial.WriteData(move + lineEnd);
//         }
//         else {
//             move = serial.ReadLine(lineEnd);
//             while (move.size() != 2+lineEnd.size() ||
//                     board.checkMove(move[0] - '0', move[1] - '0') == false) {
//                 std::cout << "\nInvalid move on the board \"" << move.substr(0, move.length()-lineEnd.length()) << "\", try again: ";
//                 move = serial.ReadLine(lineEnd);
//             }
//             std::cout << move << "\n";
//         }
//         board.move(move[0] - '0', move[1] - '0');
//     }
//     board.print();
//     std::cout << "The winner is " << board.getWinner() << "!\n";

//     return 0;
// }

#include "mainwindow.h"
#include "serial.h"
#include "board.h"
#include "myButton.h"
#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QLineEdit>
#include <iostream>
#include <thread>
#include <chrono>

void readSerial(Serial &serial, Board *board, myButton *btns[3][3], char myPlayer = 'X')
{
    // Get buffer size
    DWORD queueSize;
    while (true)
    {
        // try to read a line from the buffer
        std::string data = serial.ReadLine("\r\n");
        if (data.size() == 4)
        {
            std::string move = data.substr(0, 2);
            // check if current player is myPlayer
            if (board->getPlayer() != myPlayer)
            {
                continue;
            }
            // check if the move is valid
            if (!board->checkMove(move[0] - '0', move[1] - '0'))
            {
                continue;
            }
            board->move(move[0] - '0', move[1] - '0');
            // update the gui
            btns[move[0] - '0'][move[1] - '0']->setText(QString(myPlayer));
            // check if the game is over
            if (board->getWinner() != ' ')
            {
                // popup a message box
                // debug msg
                qDebug() << "The winner is " << QString(board->getWinner());
                QMessageBox msgBox;
                msgBox.setText("The winner is " + QString(board->getWinner()));
                // set title to "Game over"
                msgBox.setWindowTitle("Game over");
                msgBox.exec();
                // quit the app
                QApplication::quit();
            }
        }
        else
        {
            // wait for 200ms
            thSleep(200);
        }
    }
}

void boardClick(myButton *button, Board *board, Serial *serial, myButton *const btns[3][3], char myPlayer = 'O')
{
    // check if current player is myPlayer
    if (board->getPlayer() != myPlayer)
    {
        return;
    }
    // check if the move is valid
    if (!board->checkMove(button->getPosition()[0] - '0', button->getPosition()[1] - '0'))
    {
        return;
    }
    const std::string move = button->getPosition();
    // serial->WriteData(move + "\r\n");
    // make a thread that writes to serial, so that the gui doesn't freeze
    std::thread serialThread([serial, move]()
                             { serial->WriteData(move + "\r\n"); });
    serialThread.detach();
    board->move(move[0] - '0', move[1] - '0');
    button->setText(QString(myPlayer));
    // check if the game is over
    if (board->getWinner() != ' ')
    {
        // popup a message box
        // debug msg
        qDebug() << "The winner is " << QString(board->getWinner());
        QMessageBox msgBox;
        msgBox.setText("The winner is " + QString(board->getWinner()));
        // set title to "Game over"
        msgBox.setWindowTitle("Game over");
        msgBox.exec();
        // quit the app
        QApplication::quit();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::string port;
    // create a window that asks for the port
    QWidget window;
    window.setWindowTitle("Connection details");
    window.resize(200, 100);
        // text "Port:"
        QLabel *label = new QLabel("Port:", &window);
        label->setGeometry(10, 10, 180, 30);
    QLineEdit *lineEdit = new QLineEdit(&window);
    lineEdit->setGeometry(10, 10, 180, 30);
    lineEdit->setText("COM6");
    QPushButton *button = new QPushButton("Connect", &window);
    button->setGeometry(10, 50, 180, 30);
    QObject::connect(button, &QPushButton::clicked, [&window, &port, &a, &argc, &argv, &lineEdit]()
            {
                port = lineEdit->text().toStdString();
                window.close();
                a.quit();
            });
    window.show();
    a.exec();
    // an app that lets the user play tic-tac-toe with someone over serial
    MainWindow w;
    w.setWindowTitle("Tic-tac-toe");
    w.resize(300, 300);
    // Instantiate the Serial object
    Serial *serial = new Serial(port);
    // Check if the connection was successfull
    if (serial->IsConnected())
    {
        std::cout << "The port is connected\n";
    }
    else
    {
        // popup a message box
        QMessageBox msgBox;
        std::string msg = "Failed to connect to the port " + port + "\n";
        msgBox.setText(QString(msg.c_str()));
        msgBox.exec();
        return 1;
    }
    w.show();
 build-omg-Desktop_Qt_6_6_1_MinGW_64_bit-Release   // Instantiate the Board object
    char myPlayer = 'X';
    char otherPlayer = 'O';
    Board *board = new Board();
    std::string move;

    // create tic-tac-toe board
    myButton *buttons[3][3];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            buttons[i][j] = new myButton(&w);
            buttons[i][j]->setGeometry(0 + 100 * i, 0 + 100 * j, 100, 100);
            buttons[i][j]->setText(" ");
            buttons[i][j]->show();
            buttons[i][j]->setPosition(std::to_string(i) + std::to_string(j));
            QObject::connect(buttons[i][j], &QPushButton::clicked, [buttons, board, serial, i, j, myPlayer]()
                             { boardClick(buttons[i][j], board, serial, buttons, myPlayer); });
        }
    }

    // create a thread that reads from serial
    std::thread serialThread(readSerial, std::ref(*serial), board, buttons, otherPlayer);

    // clean up after quitting
    QObject::connect(&a, &QApplication::aboutToQuit, [&serialThread]()
                     { serialThread.detach(); });

    w.show();
    return a.exec();
}
