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
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(20);
    cmd.clear();
   // m_pPort->clear();
}

void Motor::STOP_TALKING_TO_MOTOR( stringstream& cmd)
{
    cmd.clear();
    cmd << "{0" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(20);
    cmd.clear();
    //m_pPort->clear();
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
}

void Motor::SetHighResolution()
    {
    TX_LOCKOUT = true;
    stringstream cmd;

    TALK_TO_MOTOR(cmd);

    cmd << "K37=10" << CRLF; // Resolution = 50,000 PPR, Speed Unit = 10
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd.clear();

    STOP_TALKING_TO_MOTOR(cmd);
    TX_LOCKOUT = false;

    }

void Motor::SetAcceleration(int Acceleration)
    {
    TX_LOCKOUT = true;
    stringstream cmd;

    TALK_TO_MOTOR(cmd);

    cmd << "A=" << Acceleration << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

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
    qDebug()<<"----";
    qDebug()<<"SetSpeed";

    double K = (50000/60);

    m_speed = K * (Speed/m_circumference);
    int pps = (int) round (m_speed);
    stringstream cmd;

    TALK_TO_MOTOR(cmd);

    cmd << "S=" << pps << CRLF;
    qDebug() << "S=" << pps << CRLF;
    logfile << cmd.str() <<endl;
    //logfile <<"setting speed" <<endl;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

    STOP_TALKING_TO_MOTOR(cmd);

    qDebug()<<"----";
    TX_LOCKOUT = false;
    }

double Motor::GetSpeed(void)
    {
        //returns revs /sec
        return (double) ((5000/60)* m_circumference)/ m_speed;
    }


void Motor::EmergencyStop(void)
    {
    TX_LOCKOUT = true;
    stringstream cmd;

     TALK_TO_MOTOR(cmd);

    cmd << "]" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

   STOP_TALKING_TO_MOTOR(cmd);

    TX_LOCKOUT = false;
    }

void Motor::Lock(void)
    {
    TX_LOCKOUT = true;
    stringstream cmd;

    cmd.clear();
    cmd << "{" << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(20);
    cmd.clear();
    //m_pPort->clear();

    //TALK_TO_MOTOR(cmd);

    cmd.clear();
    cmd << "(" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    qDebug() << cmd.str().c_str();

    cmd.clear();
    cmd << "{0" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(20);
    cmd.clear();
   // m_pPort->clear();

    // STOP_TALKING_TO_MOTOR(cmd);

    Locked = true;
    TX_LOCKOUT = false;
    }


void Motor::Free(void)
    {
    TX_LOCKOUT = true;
    stringstream cmd;

    cmd.clear();
    cmd << "{" << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(20);
    cmd.clear();
   // m_pPort->clear();

    //TALK_TO_MOTOR(cmd);

    cmd.clear();
    cmd << ")" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    qDebug() << cmd.str().c_str();

    cmd.clear();
    cmd << "{0" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(20);
    cmd.clear();
   // m_pPort->clear();

    //STOP_TALKING_TO_MOTOR(cmd);

    Locked = false;
TX_LOCKOUT = false;
    }


void Motor::Demo(void)
    {
    stringstream cmd;
    /*cmd.str("");
    cmd << "]." << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd << "P." << MotorID << "=1000000000" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd << "S." << MotorID << "=0" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd << "A." << MotorID << "=100" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    //Sleep(100);
    cmd << "S." << MotorID << "=72" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd << "^." << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    //Sleep(5000);
    cmd << "S." << MotorID << "=8000" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd << "^." << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    //Sleep(5000);
    cmd << "S." << MotorID << "=100" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd << "^." << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    //Sleep(5000);
    cmd << "S." << MotorID << "=-2000" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd << "^." << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    //Sleep(5000);
    cmd << "]." << MotorID << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    */
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



double Motor::GetPosition()
{
    if (!TX_LOCKOUT)
    {
    stringstream cmd;
    QByteArray line;
     TALK_TO_MOTOR(cmd);
    delay(5);
    cmd << "?96" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    m_pPort->waitForBytesWritten(100);
    line.clear();
//    delay(100);
// if (m_pPort->canReadLine())
qDebug() << "Go";
    if (m_pPort->waitForReadyRead(100))
    {
        line = m_pPort->readLine(100);  // return line is in form Px.1=1000
qDebug() <<line;
        line.clear();
        line = m_pPort->readLine(100);  // return line is in form Px.1=1000
qDebug() <<line;
        line.clear();
        line = m_pPort->readLine(100);  // return line is in form Px.1=1000
qDebug() <<line;
    }
    else
    {  STOP_TALKING_TO_MOTOR(cmd);
        return -1;
    }


    qDebug() << "--------------";
    qDebug()<< "line =" << line;

    QByteArray tmp = line.mid(5);  // chomp the first 5 characters to leave just the number

    qDebug() <<"Chomped first 5 chars " << tmp;

    int tpos = tmp.toLongLong();

    qDebug()<< "converted to longs"  <<tpos;

    double pos = (m_circumference * tpos) / 50000;
    qDebug()<<"converted to actual position" << pos;
   STOP_TALKING_TO_MOTOR(cmd);
    return pos;
    }

    else return -1;
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
    cmd<<"S="<<m_speed<<CRLF;
    cmd<<"P="<<tmp<<CRLF;
    cmd<<"^"<<CRLF;
    qDebug() << cmd.str().c_str();
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

   STOP_TALKING_TO_MOTOR(cmd);
    qDebug()<<"----";
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
      TALK_TO_MOTOR(cmd);
     cmd << "|2" << CRLF;
     m_pPort->write(cmd.str().c_str(),cmd.str().length());

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
