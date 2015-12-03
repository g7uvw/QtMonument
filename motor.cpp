#include "motor.h"
#include <sstream>
#include <string.h>
#include <cmath>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>

using namespace std;

#define CRLF "\x0D\x0A"


void Motor::TALK_TO_MOTOR(stringstream& cmd)
{
    cmd.clear();
    cmd << "{" << MotorID << CRLF;
    SendCommand(cmd);
    cmd.str("");
    cmd.clear();
}

void Motor::STOP_TALKING_TO_MOTOR( stringstream& cmd)
{
    cmd.clear();
    cmd << "{0" << CRLF;
    SendCommand(cmd);
    cmd.str("");
    cmd.clear();
}



Motor::Motor()
{
    logfile.open("log.txt",ios::out|ios::app);
    TX_LOCKOUT = false;
}


Motor::~Motor(void)
{
    logfile.close();
}

void Motor::Init(QSerialPort *port, uint16_t ID)
{
    m_pPort = port;
    MotorID = ID;
    SetHighResolution();
}

void Motor::SetHighResolution()
{
    TX_LOCKOUT = true;
    stringstream cmd;
    cmd.str("");
    cmd.clear();
    TALK_TO_MOTOR(cmd);
    cmd << "K37=10" << CRLF; // Resolution = 50,000 PPR, Speed Unit = 10
    SendCommand(cmd);
    STOP_TALKING_TO_MOTOR(cmd);
    TX_LOCKOUT = false;
}

void Motor::SetAcceleration(int Acceleration)
{
    TX_LOCKOUT = true;
    stringstream cmd;
    cmd.str("");
    cmd.clear();
    TALK_TO_MOTOR(cmd);
    cmd << "A=" << Acceleration << CRLF;
    SendCommand(cmd);
    STOP_TALKING_TO_MOTOR(cmd);
    TX_LOCKOUT = false;
}



void Motor::SetSpeed(double Speed)
{
    // Speed passed here is in mm/s
    // with Resolution 50000, and speed unit 10, there are
    // (5000/60) pulses per second, call this K
    // we know our spool circumference, call it C
    // The speed we want is Speed.
    // Speed in pulses, for the motor is (K/(Speed/C))
    TX_LOCKOUT = true;
    //    qDebug()<<"----";
    //    qDebug()<<"SetSpeed";

    double K = (50000/60);
    m_mmpsspeed = Speed;
    double tmp = K * (Speed/m_circumference);
    m_motorspeed = (int) round (tmp);
    stringstream cmd;
    cmd.str("");
    cmd.clear();
    TALK_TO_MOTOR(cmd);

    cmd << "S=" << m_motorspeed << CRLF;
    SendCommand(cmd);
    qDebug() << "Motor " << MotorID << ", Speed = " << m_motorspeed << CRLF;

    STOP_TALKING_TO_MOTOR(cmd);
    //qDebug()<<"----";
    TX_LOCKOUT = false;
}

double Motor::GetSpeed(void)
{
    //returns mm/s
    return m_mmpsspeed;
}


void Motor::EmergencyStop(void)
{
    TX_LOCKOUT = true;
    stringstream cmd;

    TALK_TO_MOTOR(cmd);

    cmd << "]" << CRLF;
    SendCommand(cmd);

    STOP_TALKING_TO_MOTOR(cmd);

    TX_LOCKOUT = false;
}

void Motor::Resume(void)
{
    TX_LOCKOUT = true;
    stringstream cmd;

    TALK_TO_MOTOR(cmd);

    cmd << "^" << CRLF;
    SendCommand(cmd);

    STOP_TALKING_TO_MOTOR(cmd);

    TX_LOCKOUT = false;
}

void Motor::Lock(void)
{
    TX_LOCKOUT = true;
    stringstream cmd;
    TALK_TO_MOTOR(cmd);

    cmd << "(" << CRLF;
    SendCommand(cmd);
    qDebug() << cmd.str().c_str();

    STOP_TALKING_TO_MOTOR(cmd);

    Locked = true;
    TX_LOCKOUT = false;
}

void Motor::Free(void)
{
    TX_LOCKOUT = true;
    stringstream cmd;
    TALK_TO_MOTOR(cmd);

    cmd << ")" << CRLF;
    SendCommand(cmd);
    qDebug() << cmd.str().c_str();

    STOP_TALKING_TO_MOTOR(cmd);

    Locked = false;
    TX_LOCKOUT = false;
}

void Motor::Run(long int length, int acceleration, int speed, int direction)
{
    TX_LOCKOUT = true;
    //length is in pulse counts. In 1:1 encoder mode, 50k pulses is one revolution.

    stringstream cmd;

    TALK_TO_MOTOR(cmd);

    double runspeed = speed / 10.0;
    //speed /=10;	//fix speed issue. Even with correct settinsg we're still 10x too fast.

    //stop the motor
    //cmd << "]." << MotorID << CRLF;
    //m_pPort->write(cmd.str().c_str(),cmd.str().length());

    //tell the motor the current position is 0
    //all moves are relative from here
    cmd << "|2" << CRLF;

    //set speed and acceleration
    cmd << "A=" << acceleration << CRLF;

    cmd << "S=" << runspeed << CRLF;

    //set target position
    cmd << "P=" << direction*length << CRLF;

    //go
    cmd << "^" << CRLF;

    //send commnds to motor
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

    //log comands to file
    logfile << cmd.str() <<endl;
    TX_LOCKOUT = false;
}

void Motor::Run(double pos)
{
     TX_LOCKOUT = true;
     stringstream cmd;

     double posfrac = pos/m_circumference;   // what fraction of circumfrence are we moving?

     //There are 50000 pulses per rev in the mode we're using.

     int tmp = 50000 * posfrac;

     TALK_TO_MOTOR(cmd);
     cmd<<"A=100"<<CRLF;
     cmd<<"S="<<m_motorspeed<<CRLF;
     cmd<<"P="<<tmp<<CRLF;
     cmd<<"^"<<CRLF;
     qDebug() << cmd.str().c_str();
     SendCommand(cmd);
     STOP_TALKING_TO_MOTOR(cmd);
     TX_LOCKOUT = false;
}

double Motor::GetPosition()
{
    stringstream cmd;
    QByteArray line;
    cmd.str("");
    cmd.clear();

    if (!TX_LOCKOUT)
    {
        //qDebug() << "Get Position";

        TALK_TO_MOTOR(cmd);
        cmd << "?96" << CRLF;
        line =  SendCommand(cmd);
        //qDebug() << line;
        line.clear();
        cmd.str("");
        cmd.clear();
        cmd << CRLF;
        line =  SendCommand(cmd);
        if (!line.isEmpty())
        {
            if (line.at(0) == 'P')
            {
                //qDebug()<< "I'll have a P please Bob";
                QByteArray tmp = line.mid(5);  // chomp the first 5 characters to leave just the number
                //qDebug() <<"Chomped first 5 chars " << tmp;
                std::string position = tmp.toStdString();
                int tpos = atoi (position.c_str());
                //qDebug()<< "converted to longs"  <<tpos;
                double pos = (m_circumference * tpos) / 50000;
                //qDebug()<<"converted to actual position" << pos;
                STOP_TALKING_TO_MOTOR(cmd);
                return pos;
            }

            else
            {
                STOP_TALKING_TO_MOTOR(cmd);
                return -1;
            }

        }

        else
        {
            STOP_TALKING_TO_MOTOR(cmd);
            return -1;
        }
    }
    else
    {
        STOP_TALKING_TO_MOTOR(cmd);
        return -1;
    }

}

void Motor::SetPosition(double pos)
{
    TX_LOCKOUT = true;
    qDebug()<<"----";
    qDebug()<<"SetSposition";

    stringstream cmd;

    double posfrac = pos/m_circumference;   // what fraction of circumfrence are we moving?

    //There are 50000 pulses per rev in the mode we're using.

    int tmp = 50000 * posfrac;

    TALK_TO_MOTOR(cmd);
    cmd<<"A=100"<<CRLF;
    cmd<<"S="<<m_motorspeed<<CRLF;
    cmd<<"P="<<tmp<<CRLF;
    //cmd<<"^"<<CRLF;
    qDebug() << cmd.str().c_str();
    SendCommand(cmd);
    STOP_TALKING_TO_MOTOR(cmd);
    TX_LOCKOUT = false;
}

void Motor::SetDiameter(double tmp)
{
    m_diameter = tmp;
    SetCircumference(m_diameter * 3.1415926);
}

void Motor::SetCircumference(double tmp)
{
    m_circumference = tmp;
}

void Motor::SetZero()
{
    TX_LOCKOUT = true;
    stringstream cmd;
    cmd.str("");
    cmd.clear();
    TALK_TO_MOTOR(cmd);
    cmd << "|2" << CRLF;
    SendCommand(cmd);
    STOP_TALKING_TO_MOTOR(cmd);
    TX_LOCKOUT = false;
}

void Motor::delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

QByteArray Motor::ReadData()
{
    return m_pPort->readLine();
}

QByteArray Motor::SendCommand(stringstream& cmd)
{
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(0);
    //qDebug() << "Sent command " << cmd.str().c_str();
    cmd.str("");
    cmd.clear();
    delay(10);
    return  m_pPort->readLine();
}
