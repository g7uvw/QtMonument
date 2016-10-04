#include "motor.h"
#include <sstream>
#include <string.h>
#include <cmath>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>

using namespace std;

#define CRLF "\x0D\x0A"


//bool Motor::TALK_TO_MOTOR(stringstream& cmd)
//{
//    QByteArray line;
//    cmd.clear();
//    cmd << "{" << MotorID << CRLF;
//    line = SendCommand(cmd);
//    if (!line.isEmpty())
//    {
//        cmd.str("");
//        cmd.clear();
//        return true;
//    }
//    else
//    {
//        cmd.str("");
//        cmd.clear();
//        return false;
//    }
//}

//bool Motor::STOP_TALKING_TO_MOTOR( stringstream& cmd)
//{
//    //try sending a blank command to kick the motor controller into life
//    QByteArray line;
//    cmd.clear();
//    cmd << "" << CRLF;
//    line = SendCommand(cmd);
//    if (!line.isEmpty())
//    {
//        cmd.str("");
//        cmd.clear();
//        return true;
//    }
//    else
//    {
//        cmd.str("");
//        cmd.clear();
//        return false;
//    }
//}



Motor::Motor()
{
    logfile.open("log.txt",ios::out|ios::app);
    TX_LOCKOUT = false;
}


Motor::~Motor(void)
{
    logfile.close();
}

bool Motor::Init(QSerialPort *port, uint16_t ID)
{
    m_pPort = port;
    MotorID = ID;
    return SetHighResolution();
}

bool Motor::SetHighResolution()
{
    TX_LOCKOUT = true;
    QByteArray line;
    stringstream cmd;
    cmd.str("");
    cmd.clear();

    cmd << "K37=100." << MotorID << CRLF; //Set 1:1 resolution with encoder
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }

}

bool Motor::SetAcceleration(int Acceleration)
{
    TX_LOCKOUT = true;
    QByteArray line;
    stringstream cmd;
    cmd.str("");
    cmd.clear();

    cmd << "A=" << Acceleration << "." << MotorID << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}



bool Motor::SetSpeed(double Speed)
{
    // Speed passed here is in mm/s
    // with Resolution 50000, and speed unit 10, there are
    // (50000/60) pulses per second, call this K
    // we know our spool circumference, call it C
    // The speed we want is Speed.
    // Speed in pulses, for the motor is (K/(Speed/C))

    TX_LOCKOUT = true;
    QByteArray line;
    double K = (50000/60);
    m_mmpsspeed = Speed;
    double tmp = K / (Speed/m_circumference);
    m_motorspeed = (int) round (tmp);
    stringstream cmd;
    cmd.str("");
    cmd.clear();


    cmd << "S=" << m_motorspeed << "." << MotorID << CRLF;
    qDebug() << "Motor " << MotorID << ", Speed = " << m_motorspeed << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}

bool Motor:: SetSpeed(int DegreesPerSecond)
{
    // with Resolution 50000, and speed unit 10, there are
    // (50000/60) pulses per second

    double speed = (50000.0/60.0)*((double)DegreesPerSecond/360.0);

    TX_LOCKOUT = true;
    QByteArray line;

    m_motorspeed = (int) round(speed);
    stringstream cmd;
    cmd.str("");
    cmd.clear();

    cmd << "S=" << m_motorspeed << "." << MotorID << CRLF;
    qDebug() << "Rotation " << MotorID << ", Speed = " << m_motorspeed << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}


double Motor::GetSpeed(void)
{
    //returns mm/s
    return m_mmpsspeed;
}


bool Motor::EmergencyStop(void)
{
    TX_LOCKOUT = true;
    QByteArray line;
    stringstream cmd;



    cmd << "]." << MotorID << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}

bool Motor::Resume(void)
{
    TX_LOCKOUT = true;
    QByteArray line;
    stringstream cmd;



    cmd << "^." << MotorID << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}

bool Motor::Lock(void)
{
    TX_LOCKOUT = true;
    QByteArray line;
    stringstream cmd;


    cmd << "(." << MotorID << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}

bool Motor::Free(void)
{
    TX_LOCKOUT = true;
    QByteArray line;
    stringstream cmd;


    cmd << ")." << MotorID << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}



bool Motor::Run(double pos)
{
     TX_LOCKOUT = true;
     QByteArray line;
     stringstream cmd;

     double posfrac = pos/m_circumference;   // what fraction of circumfrence are we moving?

     //There are 50000 pulses per rev in the mode we're using.

     int tmp = 50000 * posfrac;


     cmd<<"A=100" << "." << MotorID << CRLF;
     cmd<<"S="<<m_motorspeed<< "." << MotorID << CRLF;
     cmd<<"P="<<tmp<< "." << MotorID << CRLF;
     cmd<<"^"<< "." << MotorID << CRLF;
     qDebug() << cmd.str().c_str();
     line = SendCommand(cmd);
     if (!line.isEmpty())
     {

       TX_LOCKOUT = false;
       return false;

     }
     else
     {

         TX_LOCKOUT = false;
         return true;
     }
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


        cmd << "?96"<< "." << MotorID << CRLF;
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
                //STOP_TALKING_TO_MOTOR(cmd);
                return pos;
            }

            else
            {
                //STOP_TALKING_TO_MOTOR(cmd);
                return -1;
            }

        }

        else
        {
            //STOP_TALKING_TO_MOTOR(cmd);
            return -1;
        }
    }
    else
    {
        return -1;
    }

}

bool Motor::SetPosition(double pos)
{
    TX_LOCKOUT = true;
    QByteArray line;
    qDebug()<<"----";
    qDebug()<<"SetSposition";

    stringstream cmd;

    double posfrac = pos/m_circumference;   // what fraction of circumfrence are we moving?

    //There are 50000 pulses per rev in the mode we're using.

    int tmp = 50000 * posfrac;


    cmd<<"A=100"<< "." << MotorID << CRLF;
    cmd<<"S="<<m_motorspeed<< "." << MotorID << CRLF;
    cmd<<"P="<<tmp<< "." << MotorID << CRLF;

    qDebug() << cmd.str().c_str();
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
}

bool Motor::SetPosition(int Degrees)
{
    // 50000 pulses per revolution in this mode
    // 360 degrees per revolution.
    // position sent to motor is then 50,000 * position wanted / 360

    m_motorpos = 50000 * (double) (Degrees/360.0);

    TX_LOCKOUT = true;
    QByteArray line;
    qDebug()<<"----";
    qDebug()<<"Setposition rotation";
    stringstream cmd;

    cmd<<"A=100"<< "." << MotorID << CRLF;
    cmd<<"S="<<m_motorspeed<< "." << MotorID << CRLF;
    cmd<<"P="<<m_motorpos<< "." << MotorID << CRLF;

    qDebug() << cmd.str().c_str();
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }


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

bool Motor::SetZero()
{
    TX_LOCKOUT = true;
    QByteArray line;
    stringstream cmd;
    cmd.str("");
    cmd.clear();

    cmd << "|2" << "." << MotorID << CRLF;
    line = SendCommand(cmd);
    if (!line.isEmpty())
    {

      TX_LOCKOUT = false;
      return false;

    }
    else
    {

        TX_LOCKOUT = false;
        return true;
    }
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
    if (m_pPort->canReadLine())
        return m_pPort->readLine();
    else
        return QByteArray();
}

QByteArray Motor::SendCommand(stringstream& cmd)
{
    m_pPort->write(cmd.str().c_str(),cmd.str().length());
    if (m_pPort->waitForBytesWritten(10))
    {
        cmd.str("");
        cmd.clear();
        delay(10);
        return  m_pPort->readLine();
    }
    else
        return QByteArray();
}
