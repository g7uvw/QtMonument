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
    SerialOpen = false;
    serial = new QSerialPort(this);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    settings = new SettingsDialog;
    ui->setupUi(this);

    // two motors

    Motor lower();
    Motor upper();

    serialstatus = new QLabel(this);
    motor1status = new QLabel(this);
    motor2status = new QLabel(this);

    ui->statusBar->addPermanentWidget(serialstatus,1);
    ui->statusBar->addPermanentWidget(motor1status,1);
    ui->statusBar->addPermanentWidget(motor2status,1);
    serialstatus->setText("Serial : Not connected");

    //update timer
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(GetPositions()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetPositions()
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

        openSerialPort();
        lower.Init(serial,1);
        upper.Init(serial,2);

        upper.Free();
        lower.Free();
        motor1status->setText("Motor 1 : Free");
        motor2status->setText("Motor 2 : Free");

        E_STOPPED = false;

        lower.SetDiameter(ui->lower_diameter_spin->value());
        upper.SetDiameter(ui->upper_diameter_spin->value());

       timer->start(1000);

}

void MainWindow::openSerialPort()
{
        if (serial->isOpen())
                serial->close();
            SettingsDialog::Settings p = settings->settings();
            serial->setPortName(p.name);
            serial->setBaudRate(19200);
            serial->setDataBits(p.dataBits);
            serial->setParity(p.parity);
            serial->setStopBits(p.stopBits);
            serial->setFlowControl(p.flowControl);
            if (serial->open(QIODevice::ReadWrite))
            {
                   SerialOpen = true;
                   serialstatus->setText("Serial : Connected");
                   qDebug()<<"Port open";
            }
            else
            {
                QMessageBox::critical(this, tr("Error Connecting to the motors - try restarting everything!"), serial->errorString());
            }

}

void MainWindow::on_action_Exit_triggered()
{
    if (serial->isOpen())
        serial->close();
    qApp->exit();
}

void MainWindow::readData()
{

}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
            QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
            closeSerialPort();
        }
}

void MainWindow::closeSerialPort()
{

}

void MainWindow::on_actionComms_Settings_triggered()
{
    settings->show();
    // show serial settings

}

void MainWindow::on_lower_speed_spin_valueChanged(int arg1)
{
    if (!serial->isOpen())
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
    if (!serial->isOpen())
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }

    if (checked)
    {
        lower.Lock();
        motor1status->setText("Motor 1 - Enabled");
    }
    else
    {
        lower.Free();
        motor1status->setText("Motor 1 - Free");
    }
}

void MainWindow::on_upper_enable_toggled(bool checked)
{
    if (!serial->isOpen())
    {
        QMessageBox::critical(this, tr("You're not connected to the motors!"), "Check the communications settings");
        return;
    }

    if (checked)
    {
        upper.Lock();
        motor2status->setText("Motor 2 - Enabled");
    }
    else
    {
        upper.Free();
        motor2status->setText("Motor 2 - Free");
    }
}

void MainWindow::on_lower_pos_spin_valueChanged(int arg1)
{
    if (!serial->isOpen())
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
   ui->upper_pos_spin->setValue(0);
   lower.SetZero();
   upper.SetZero();
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
   // upper.Lock();
   // lower.Lock();
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
    ui->jog_speed_spin->setValue(0.0);
}

void MainWindow::on_jog_pos_spin_valueChanged(double arg1)
{
    upper.SetPosition(ui->lower_pos_spin->value() + arg1);
    lower.SetPosition(ui->lower_pos_spin->value() + arg1);
    lower.Run(ui->lower_pos_spin->value() + arg1);
    upper.Run(ui->lower_pos_spin->value() + arg1);
}

void MainWindow::on_jog_pos_save_clicked()
{
    saved_pos = ui->lower_pos_spin->value();
}

void MainWindow::on_jog_pos_restore_clicked()
{
    ui->lower_pos_spin->setValue(saved_pos);
}
