#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "serial.h"
#include "motor.h"
#include <QtWidgets>
#include <QMessageBox>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    //serial stuff
    WireSerialOpen = false;
    RotationSerialOpen = false;
    serial_wire = new QSerialPort(this);
    serial_rotation = new QSerialPort(this);
    connect(serial_wire, SIGNAL(error(QSerialPort::SerialPortError)), this,SLOT(handleError_wire(QSerialPort::SerialPortError)));
    connect(serial_wire, SIGNAL(readyRead()), this, SLOT(readData_wire()));

    connect( serial_rotation, SIGNAL(error(QSerialPort::SerialPortError)), this,SLOT(handleError_rotation(QSerialPort::SerialPortError)));
    connect( serial_rotation, SIGNAL(readyRead()), this, SLOT(readData_rotation()));

    settings_wire = new SettingsDialog;
    settings_rotation = new SettingsDialog;
    ui->setupUi(this);

    // three motors

    Motor lower;
    Motor upper;
    Motor Rotation;

    serialstatus_wire = new QLabel(this);
    serialstatus_rotation = new QLabel(this);
    motor1status = new QLabel(this);
    motor2status = new QLabel(this);
    rotationstatus = new QLabel(this);

    ui->statusBar->addPermanentWidget(serialstatus_wire,1);
    ui->statusBar->addPermanentWidget(motor1status,1);
    ui->statusBar->addPermanentWidget(motor2status,1);
    serialstatus_wire->setText("Serial1 : Not connected");
    ui->statusBar->addPermanentWidget(serialstatus_rotation,1);
    ui->statusBar->addPermanentWidget(rotationstatus,1);
    serialstatus_rotation->setText("Serial2 : Not connected");
    qApp->processEvents();
    //update timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(GetPositions_wire()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetPositions_wire()
{
    qApp->processEvents();
    if (WireSerialOpen)
    {
         lp = lower.GetPosition();
         up = upper.GetPosition();
         //qDebug()<<lp;
         if(lp !=0)
         {
            ui->lower_distance->setValue(lp+0.01);
            ui->upper_distance->setValue(up+0.01);
         }
    }

qApp->processEvents();
}

void MainWindow::on_actionConnect_to_motors_triggered()
{

    openSerialPort_wire();

    if(!lower.Init(serial_wire, 1))
        QMessageBox::critical(this, tr("Error Initialising Motor 1"), tr("Error Initialising Motor 1, check power and restart software.") );
    if (!upper.Init(serial_wire, 2))
        QMessageBox::critical(this, tr("Error Initialising Motor 2"), tr("Error Initialising Motor 2, check power and restart software.") );

    if(!upper.Free())
        QMessageBox::critical(this, tr("Error Freeing Motor 2"), tr("Error Freeing Motor 2, try again") );
    if(!lower.Free())
        QMessageBox::critical(this, tr("Error Freeing Motor 1"), tr("Error Freeing Motor 1, try again") );

    motor1status->setText("Motor 1 : Free");
    motor2status->setText("Motor 2 : Free");

    E_STOPPED = false;

    lower.SetDiameter(ui->lower_diameter_spin->value());
    upper.SetDiameter(ui->upper_diameter_spin->value());

    timer->start(10);

}

void MainWindow::openSerialPort_wire()
{
        if (serial_wire->isOpen())
                serial_wire->close();
            SettingsDialog::Settings p = settings_wire->settings();
            serial_wire->setPortName(p.name);
            serial_wire->setBaudRate(38400); //19200
            serial_wire->setDataBits(p.dataBits);
            serial_wire->setParity(p.parity);
            serial_wire->setStopBits(p.stopBits);
            serial_wire->setFlowControl(p.flowControl);
            if (serial_wire->open(QIODevice::ReadWrite))
            {
                   WireSerialOpen = true;
                   serialstatus_wire->setText("Serial1 : Connected");
                   qDebug()<<"Port open";
            }
            else
            {
                QMessageBox::critical(this, tr("Error Connecting to the motors - try restarting everything!"), serial_wire->errorString());
            }

}

void MainWindow::openSerialPort_rotation()
{
    if (WireSerialOpen)
            serial_rotation->close();
        SettingsDialog::Settings q = settings_rotation->settings();
        serial_rotation->setPortName(q.name);
        serial_rotation->setBaudRate(19200);
        serial_rotation->setDataBits(q.dataBits);
        serial_rotation->setParity(q.parity);
        serial_rotation->setStopBits(q.stopBits);
        serial_rotation->setFlowControl(q.flowControl);
        if (serial_rotation->open(QIODevice::ReadWrite))
        {
            RotationSerialOpen = true;
            serialstatus_rotation->setText("Serial2: Connected");
               qDebug()<<"Rotation Port open";
        }
        else
        {
            QMessageBox::critical(this, tr("Error Connecting to the rotation stage - try restarting everything!"), serial_rotation->errorString());
        }
}


void MainWindow::on_action_Exit_triggered()
{
    closeSerialPort_wire();
    closeSerialPort_rotation();
    qApp->exit();
}

void MainWindow::readData_wire()
{

}

void MainWindow::handleError_wire(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
            QMessageBox::critical(this, tr("Critical Error"), serial_wire->errorString());
            closeSerialPort_wire();
        }
}

void MainWindow::handleError_rotation(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
            QMessageBox::critical(this, tr("Critical Error"), serial_rotation->errorString());
            closeSerialPort_rotation();
        }
}


void MainWindow::closeSerialPort_wire()
{
    if (WireSerialOpen)
        serial_wire->close();
    serialstatus_wire->setText("Serial1: Disconnected");
}

void MainWindow::closeSerialPort_rotation()
{
    if(RotationSerialOpen)
        serial_rotation->close();
    serialstatus_rotation->setText("Serial2: Disconnected");
}


void MainWindow::on_actionComms_wire_Settings_triggered()
{
    settings_wire->show();
}



void MainWindow::on_lower_speed_spin_valueChanged(int arg1)
{
    if (!WireSerialOpen)
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }

    lower.WireSetSpeed((double)arg1); //cast as double here so we don't hit the int handler used for rotation stage.
    // nasty cludgy hack. Sorry future me.
    float uppertmp = arg1 * (lower.m_circumference / upper.m_circumference);
    ui->upper_speed_spin->setValue( arg1 * (lower.m_circumference / upper.m_circumference));
    upper.WireSetSpeed(arg1 * (lower.m_circumference / upper.m_circumference));
}



void MainWindow::on_lower_enable_toggled(bool checked)
{
    if (!WireSerialOpen)
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }

    if (checked)
    {
        if (!lower.Lock())
           {
            QMessageBox::critical(this, tr("Error Locking Motor 1"), tr("Error Locking Motor 1, try again") );
        }
        else
        {
            motor1status->setText("Motor 1 - Enabled");
        }

    }
    else
    {
        if (!lower.Free())
        {
            QMessageBox::critical(this, tr("Error Freeing Motor 1"), tr("Error Freeing Motor 1, try again") );
        }
        else
        {
         motor1status->setText("Motor 1 - Free");
        }
    }
}

void MainWindow::on_upper_enable_toggled(bool checked)
{
    if (!WireSerialOpen)
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }

    if (checked)
    {
        if (!upper.Lock())
        {
            QMessageBox::critical(this, tr("Error Locking Motor 2"), tr("Error Locking Motor 2, try again") );
        }
        else
        {
            motor2status->setText("Motor 2 - Enabled");
        }
    }
    else
    {
        if (!upper.Free())
        {
            QMessageBox::critical(this, tr("Error Freeing Motor 2"), tr("Error Freeing Motor 2, try again") );
        }
        else
        {
            motor2status->setText("Motor 2 - Free");
        }
    }
}

void MainWindow::on_lower_pos_spin_valueChanged(int arg1)
{
    if (!WireSerialOpen)
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }
    lower.WireSetSpeed(ui->lower_speed_spin->value());
    lower.SetDiameter(ui->lower_diameter_spin->value());
    lower.WireSetPosition(arg1);
}

void MainWindow::on_upper_diameter_spin_valueChanged(double arg1)
{
    upper.SetDiameter(arg1);
}

void MainWindow::on_Lower_Pos_Zero_clicked()
{
   ui->lower_pos_spin->setValue(0);
   lower.SetZero();
}

void MainWindow::on_EMERGENCY_STOP_clicked()
{
    if (E_STOPPED)
    {
        ui->EMERGENCY_STOP->setText("Emergency Stop");
        if (WireSerialOpen)
        {
            lower.Resume();
            upper.Resume();
        }
        if (RotationSerialOpen)
            rotation.Resume();
        E_STOPPED = false;
    }
    else
    {
       if (WireSerialOpen)
       {
            upper.EmergencyStop();
            lower.EmergencyStop();
       }
       if (RotationSerialOpen)
            rotation.EmergencyStop();
        E_STOPPED = true;
        ui->EMERGENCY_STOP->setText("Resume...");
    }
}

void MainWindow::on_upper_speed_spin_editingFinished()
{

}

void MainWindow::on_RUN_clicked()
{
    if (WireSerialOpen)
    {
        lower.Run(ui->lower_pos_spin->value());
        upper.Run(ui->lower_pos_spin->value());
    }
}

void MainWindow::on_lower_diameter_spin_valueChanged(double arg1)
{
    lower.SetDiameter(arg1);
}

void MainWindow::on_jog_speed_spin_valueChanged(double arg1)
{
    upper.WireSetSpeed(upper.GetSpeed()+ arg1);
}

void MainWindow::on_jog_speed_save_clicked()
{
    saved_speed = (upper.GetSpeed()+ ui->jog_speed_spin->value());
}

void MainWindow::on_jog_speed_restore_clicked()
{
    upper.WireSetSpeed(saved_speed);
    ui->upper_speed_spin->setValue(saved_speed);
}

void MainWindow::readData_rotation()
{
int wibble = rotation.GetPosition();

}

void MainWindow::on_actionRotation_Comms_Settings_triggered()
{
    settings_rotation->show();
}

void MainWindow::on_actionConnect_to_Rotation_Stage_triggered()
{
    openSerialPort_rotation();

    if(!rotation.Init(serial_rotation,1))
        QMessageBox::critical(this, tr("Error Initialising rotation stage"), tr("Error Initialising rotation stage, check power and restart software.") );

    rotation.Free();
}

void MainWindow::on_Rotation_speed_spin_valueChanged(int arg1)
{
    rotation.WireSetSpeed(arg1);
}

void MainWindow::on_Rotation_pos_spin_valueChanged(int arg1)
{

}



void MainWindow::on_pushButton_clicked(bool checked)
{
    if (!RotationSerialOpen)
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }

    if (checked)
    {
        if (!rotation.Lock())
           {
            QMessageBox::critical(this, tr("Error Locking Turntable Motor 1"), tr("Error Locking Turntable Motor 1, try again") );
        }
        rotationstatus->setText("Enabled.");

    }
    else
    {
        if (!rotation.Free())
        {
            QMessageBox::critical(this, tr("Error FreeingTurntable Motor 1"), tr("Error Freeing Turntable Motor 1, try again") );
        }
        rotationstatus->setText("Free.");
    }
}

void MainWindow::on_actionComms_Settings_triggered()
{
    settings_wire->show();
}

void MainWindow::on_Rotation_pos_spin_editingFinished()
{
    rotation.RotationSetPosition(ui->Rotation_pos_spin->value());
    rotation.Rotate();
}
