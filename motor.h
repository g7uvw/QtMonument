#pragma once
#include "serial.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>

class Motor
    {
    public:
        Motor(QSerialPort *port, uint16_t ID);
        //Motor( int portnum, char ID);
        virtual ~Motor(void);
        bool SetAcceleration(int Acceleration);
        bool SetHighResolution();
        bool EmergencyStop(void);
        bool SetSpeed(double Speed);
        bool SetDirection(bool Clockwise);
        double GetSpeed(void);
        void Init(void);
        long int GetPosition();

    public:
        int MotorID;
    protected:
        // Pointer to serial port
        QSerialPort *m_pPort;
        int m_portnum;
    public:
        bool Lock(void);
        bool Free(void);
        bool Locked;
        std::string init1;
        std::string init2;
        std::string init3;
        int speed;

        void Demo(void);
        void Run(long int length, int acceleration, int speed, int direction);
        std::ofstream logfile;

    };

#ifndef MOTOR
#define MOTOR

#endif // MOTOR

