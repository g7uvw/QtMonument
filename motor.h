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
        //Motor(QSerialPort *port, uint16_t ID);
        //Motor( int portnum, char ID);
        virtual ~Motor(void);
        void SetAcceleration(int Acceleration);
        void SetHighResolution();
        void EmergencyStop(void);
        void SetSpeed(double Speed);
        void SetDirection(bool Clockwise);
        double GetSpeed(void);
        void   SetPosition(double);
        void Init(QSerialPort *port, uint16_t ID);
        double GetPosition();
        void SetDiameter(double);
        void SetCircumference(double);
        void SetZero();
        void delay( int millisecondsToWait );
        QByteArray ReadData(void);
        QByteArray SendCommand(std::stringstream& cmd);

    public:
        int MotorID;
        QSerialPort *m_pPort;
        double m_diameter;
        double m_circumference;
        bool TX_LOCKOUT;
        void TALK_TO_MOTOR(std::stringstream& cmd);
        void STOP_TALKING_TO_MOTOR(std::stringstream& cmd);
    protected:

    public:
        void Lock(void);
        void Free(void);
        bool Locked;
        double  m_speed;
        void Run(long int length, int acceleration, int speed, int direction);
        std::ofstream logfile;

        int SpoolDiameter;

    };

#ifndef MOTOR
#define MOTOR

#endif // MOTOR

