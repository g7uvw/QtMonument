#include "motor.h"
#include <sstream>
#include <string.h>
#include <cmath>
#include <QDebug>

#define CRLF "\x0D\x0A"


#define TALK_TO_MOTOR() \
    cmd.clear(); \
    cmd << "{" << MotorID << CRLF;   \
    m_pPort->write(cmd.str().c_str(),cmd.str().length()); \
    cmd.clear();

#define STOP_TALKING_TO_MOTOR() \
    cmd.clear(); \
    cmd << "{0" << CRLF;   \
    m_pPort->write(cmd.str().c_str(),cmd.str().length()); \
    cmd.clear();

using namespace std;

Motor::Motor()

//Motor::Motor(QSerialPort *port, uint16_t ID)
//    : m_pPort(port)
//    , MotorID(ID)
    {
    logfile.open("log.txt",ios::out|ios::app);
    //logfile <<"Motor "<<ID<<" Open for communication"<<endl;

   // char sRxBuf[300]={};
   // stringstream cmd;
   // cmd << "K37." << MotorID << CRLF;
  //  m_pPort->write(cmd.str().c_str(),cmd.str().length());
   // m_pPort->read(sRxBuf,20,200);


    //cmd.clear();
   // cmd.str("");
   // cmd << "K14." << MotorID << "\x0D\x0A";
   // m_pPort->write(cmd.str().c_str(),cmd.str().length());
    //m_pPort->read(sRxBuf,200,2000);


    //dump motor params for debugging.
   // cmd.str("");
   // cmd << "%100" << CRLF;
   // m_pPort->write(cmd.str().c_str(),cmd.str().length());
    //m_pPort->read(sRxBuf,200,2000);
   // init3 = sRxBuf;

    }


Motor::~Motor(void)
    {
        logfile.close();
    }

void Motor::Init(QSerialPort *port, uint16_t ID)
{
    m_pPort = port;
    MotorID = ID;
   // stringstream cmd;

   // TALK_TO_MOTOR()

   // cmd << "|2" << CRLF;        //Set current postion to be origin
   // m_pPort->write(cmd.str().c_str(),cmd.str().length());
   // cmd.clear();

   // STOP_TALKING_TO_MOTOR()
}

bool Motor::SetHighResolution()
    {

    stringstream cmd;

    TALK_TO_MOTOR()

    cmd << "K37=10" << CRLF; // Resolution = 50,000 PPR, Speed Unit = 10
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    cmd.clear();

    STOP_TALKING_TO_MOTOR()

    return false;
    }

bool Motor::SetAcceleration(int Acceleration)
    {
    stringstream cmd;

    TALK_TO_MOTOR()

    cmd << "A=" << Acceleration << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

    STOP_TALKING_TO_MOTOR()

    return false;
    }



bool Motor::SetSpeed(double Speed)
    {
    // Speed passed here is in mm/s
    // with Resolution 50000, and speed unit 10, there are
    // (5000/60) pulses per second, call this K
    // we know our spool circumference, call it C
    // The speed we want is Speed.
    // Speed in pulses, for the motor is (K/(Speed/C))

    double K = (50000/60);

    m_speed = K * (Speed/m_circumference);
    int pps = (int) round (m_speed);
    stringstream cmd;

    TALK_TO_MOTOR()

    cmd << "S=" << pps << CRLF;
    qDebug() << "S=" << pps << CRLF;
    logfile << cmd.str() <<endl;
    //logfile <<"setting speed" <<endl;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

    STOP_TALKING_TO_MOTOR()
    return false;
    }

double Motor::GetSpeed(void)
    {
        //returns revs /sec
        return (double) ((5000/60)* m_circumference)/ m_speed;
    }


bool Motor::EmergencyStop(void)
    {
    stringstream cmd;

    TALK_TO_MOTOR()

    cmd << "]" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

    STOP_TALKING_TO_MOTOR()

    return false;
    }

bool Motor::Lock(void)
    {
    stringstream cmd;

    TALK_TO_MOTOR()

    cmd << "(" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    qDebug() << cmd.str().c_str();

    STOP_TALKING_TO_MOTOR()

    Locked = true;
    return false;
    }


bool Motor::Free(void)
    {
    stringstream cmd;

    TALK_TO_MOTOR()

    cmd.str("");
    cmd << ")" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    qDebug() << cmd.str().c_str();

    STOP_TALKING_TO_MOTOR()

    Locked = false;
    return false;
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
    //length is in pulse counts. In 1:1 encoder mode, 50k pulses is one revolution.

    stringstream cmd;

    TALK_TO_MOTOR()

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
    }



double Motor::GetPosition()
{
    stringstream cmd;
    QByteArray line;
    TALK_TO_MOTOR()

    cmd << "?96" << CRLF;
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    if (m_pPort->canReadLine())
    {
        line = m_pPort->readLine(100);  // return line is in form Px.1=1000
    }
    else return -1;

    QByteArray tmp = line.mid(5);  // chomp the first 5 characters to leave just the number
    int tpos = tmp.toLongLong();

    double pos = (m_circumference * tpos) / 50000;

    return pos;
}

void Motor::SetPosition(double pos)
{
    stringstream cmd;

    double posfrac = pos/m_circumference;   // what fraction of circumfrence are we moving?

    //There are 50000 pulses per rev in the mode we're using.

    int tmp = 50000 * posfrac;

    TALK_TO_MOTOR()
    cmd<<"A=100"<<CRLF;
    cmd<<"S="<<m_speed<<CRLF;
    cmd<<"P="<<tmp<<CRLF;
    cmd<<"^"<<CRLF;
    qDebug() << cmd.str().c_str();
    m_pPort->write(cmd.str().c_str(),cmd.str().length());

    STOP_TALKING_TO_MOTOR()
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
     stringstream cmd;
     TALK_TO_MOTOR()
     cmd << "|2" << CRLF;
     m_pPort->write(cmd.str().c_str(),cmd.str().length());

     STOP_TALKING_TO_MOTOR()
}
