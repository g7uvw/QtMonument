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
        bool SetAcceleration(int Acceleration);
        bool SetHighResolution();
        bool EmergencyStop(void);
        bool SetSpeed(double Speed);
        bool SetDirection(bool Clockwise);
        double GetSpeed(void);
        void   SetPosition(double);
        void Init(QSerialPort *port, uint16_t ID);
        double GetPosition();
        void SetDiameter(double);
        void SetCircumference(double);
        void SetZero();

    public:
        int MotorID;
        QSerialPort *m_pPort;
        double m_diameter;
        double m_circumference;
    protected:

    public:
        bool Lock(void);
        bool Free(void);
        bool Locked;
        double  m_speed;
        void Demo(void);
        void Run(long int length, int acceleration, int speed, int direction);
        std::ofstream logfile;

        int SpoolDiameter;

    };

#ifndef MOTOR
#define MOTOR

#endif // MOTOR

