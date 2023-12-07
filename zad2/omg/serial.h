#pragma once

#include <windows.h>
#include <iostream>

class Serial
{
    private:
        //Serial comm handler
        HANDLE hSerial;
        //Connection status
        bool connected;
        //Get various information about the connection
        COMSTAT status;
        //Keep track of last errors
        DWORD errors;

    public:
        //Initialize Serial communication with the given COM port
        Serial();
        Serial(const std::string portName);
        bool start(const std::string portName);
        void stop();
        //Close the connection
        //NOTA: for some reason you can't connect again before exiting
        //the program and running it again
        ~Serial();
        //Read data in a buffer, if nbChar is greater than the
        //maximum number of bytes available, it will return only the
        //bytes available. The function return -1 when nothing could
        //be read, the number of bytes actually read.
        int ReadData(char *buffer, unsigned int nbChar);
        std::string ReadData(unsigned int nChars);
        std::string ReadDataLength(unsigned int length, uint8_t sleep=1);
        std::string ReadLine(const std::string end="\r\n", uint8_t sleep=1);
        //Writes data from a buffer through the Serial connection
        //return true on success.
        bool WriteData(char *buffer, unsigned int nbChar);
        bool WriteData(const std::string data);
        //Check if we are actually connected
        bool IsConnected() const;
        DWORD queueSize();

};

void thSleep(uint32_t ms);
