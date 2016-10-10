#pragma once
#include "serial.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>

class Motor
    {
    public:
        Motor();
        virtual ~Motor(void);
        int MotorID;

        bool SetAcceleration(int Acceleration);
        bool SetHighResolution();
        bool EmergencyStop(void);
        bool Resume(void);
        bool SetSpeed(double Speed);
        bool SetSpeed(int DegreesPerSecond);
        bool SetDirection(bool Clockwise);
        bool SetZero();
        bool SetPosition(double);
        bool SetPosition(int Degrees);
        bool Init(QSerialPort *port, uint16_t ID);

//        bool STOP_TALKING_TO_MOTOR(std::stringstream& cmd);
        bool Run(double pos);
        bool Rotate();
        bool Lock(void);
        bool Free(void);

        double GetSpeed(void);
        double GetPosition();

        void SetDiameter(double);
        void SetCircumference(double);

        void delay( int millisecondsToWait );
        QByteArray ReadData(void);
        QByteArray SendCommand(std::stringstream& cmd);

    public:

        QSerialPort *m_pPort;
        double m_diameter;
        double m_circumference;
        bool TX_LOCKOUT;
        bool Locked;
        int  m_motorspeed;
        int m_motorpos;
        double m_mmpsspeed;
        std::ofstream logfile;
        unsigned int SpoolDiameter;
        static const int Resolution = 50000;
        static const int SpeedUnit = 1;

    };

#ifndef MOTOR
#define MOTOR

#endif // MOTOR

