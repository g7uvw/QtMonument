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
    serial_wire = new QSerialPort(this);
    connect(serial_wire, SIGNAL(error(QSerialPort::SerialPortError)), this,SLOT(handleError_wire(QSerialPort::SerialPortError)));
    connect(serial_wire, SIGNAL(readyRead()), this, SLOT(readData_wire()));
    settings_wire = new SettingsDialog;
    ui->setupUi(this);

    // two motors

    Motor lower;
    Motor upper;

    serialstatus_wire = new QLabel(this);
    motor1status = new QLabel(this);
    motor2status = new QLabel(this);

    ui->statusBar->addPermanentWidget(serialstatus_wire,1);
    ui->statusBar->addPermanentWidget(motor1status,1);
    ui->statusBar->addPermanentWidget(motor2status,1);
    serialstatus_wire->setText("Serial : Not connected");

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
    //qDebug()<<lower.GetPosition();
    int lp = lower.GetPosition();
    int up = upper.GetPosition();
    if (lp != -1)
    {
        ui->lower_distance->setValue(lp);
    }
    if (up != -1)
    {
        ui->upper_distance->setValue(up);
    }
}

void MainWindow::on_actionConnect_to_motors_triggered()
{

    openSerialPort_wire();

    if(!lower.Init(serial_wire,1))
        QMessageBox::critical(this, tr("Error Initialising Motor 1"), tr("Error Initialising Motor 1, check power and restart software.") );
    if (!upper.Init(serial_wire,2))
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

    timer->start(500);

}

void MainWindow::openSerialPort_wire()
{
        if (serial_wire->isOpen())
                serial_wire->close();
            SettingsDialog::Settings p = settings_wire->settings();
            serial_wire->setPortName(p.name);
            serial_wire->setBaudRate(19200);
            serial_wire->setDataBits(p.dataBits);
            serial_wire->setParity(p.parity);
            serial_wire->setStopBits(p.stopBits);
            serial_wire->setFlowControl(p.flowControl);
            if (serial_wire->open(QIODevice::ReadWrite))
            {
                   WireSerialOpen = true;
                   serialstatus_wire->setText("Serial : Connected");
                   qDebug()<<"Port open";
            }
            else
            {
                QMessageBox::critical(this, tr("Error Connecting to the motors - try restarting everything!"), serial_wire->errorString());
            }

}

void MainWindow::on_action_Exit_triggered()
{
    if (serial_wire->isOpen())
        serial_wire->close();
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

void MainWindow::closeSerialPort_wire()
{

}

void MainWindow::on_actionComms_wire_Settings_triggered()
{
    settings_wire->show();
    // show serial settings

}

void MainWindow::on_lower_speed_spin_valueChanged(int arg1)
{
    if (!serial_wire->isOpen())
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }

    lower.SetSpeed(arg1);
    float uppertmp = arg1 * (lower.m_circumference / upper.m_circumference);
    ui->upper_speed_spin->setValue( arg1 * (lower.m_circumference / upper.m_circumference));
    upper.SetSpeed(arg1 * (lower.m_circumference / upper.m_circumference));
}



void MainWindow::on_lower_enable_toggled(bool checked)
{
    if (!serial_wire->isOpen())
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
    if (!serial_wire->isOpen())
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
    if (!serial_wire->isOpen())
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }
    lower.SetSpeed(ui->lower_speed_spin->value());
    lower.SetDiameter(ui->lower_diameter_spin->value());
    lower.SetPosition(arg1);
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
        lower.Resume();
        upper.Resume();
        E_STOPPED = false;
    }
    else
    {
        upper.EmergencyStop();
        lower.EmergencyStop();
        E_STOPPED = true;
        ui->EMERGENCY_STOP->setText("Resume...");
    }
}

void MainWindow::on_upper_speed_spin_editingFinished()
{

}

void MainWindow::on_RUN_clicked()
{
    lower.Run(ui->lower_pos_spin->value());
    upper.Run(ui->lower_pos_spin->value());
}



void MainWindow::on_lower_diameter_spin_valueChanged(double arg1)
{
    lower.SetDiameter(arg1);
}

void MainWindow::on_jog_speed_spin_valueChanged(double arg1)
{
    upper.SetSpeed(upper.GetSpeed()+ arg1);
}

void MainWindow::on_jog_speed_save_clicked()
{
    saved_speed = (upper.GetSpeed()+ ui->jog_speed_spin->value());
}

void MainWindow::on_jog_speed_restore_clicked()
{
    upper.SetSpeed(saved_speed);
    ui->upper_speed_spin->setValue(saved_speed);
}


// new functions move to good places

void MainWindow::openSerialPort_rotation()
{
}

void MainWindow::closeSerialPort_rotation()
{

}

void MainWindow::readData_Rotation()
{

}


void MainWindow::handleError_rotation(QSerialPort::SerialPortError error)
{

}
