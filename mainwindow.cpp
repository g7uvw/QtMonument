#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include "serial.h"
#include "motor.h"
#include <QtWidgets>

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionConnect_to_motors_triggered()
{

        openSerialPort();
        lower.Init(serial,1);
        upper.Init(serial,2);
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
    lower.SpoolDiameter = ui->lower_diameter_spin->value();
    double circumference = 3.1415926* lower.SpoolDiameter;
    lower.SetSpeed(arg1 / circumference);

    ui->upper_speed_spin->setValue( lower.m_speed * (ui->lower_diameter_spin->value() / ui->upper_diameter_spin->value()));
}



void MainWindow::on_lower_enable_toggled(bool checked)
{
    if (checked)
        lower.Lock();
    else
        lower.Free();
}

void MainWindow::on_upper_enable_toggled(bool checked)
{
    if (checked)
        upper.Lock();
    else
        upper.Free();
}

void MainWindow::on_lower_pos_spin_valueChanged(int arg1)
{
    qDebug() << "Setting Position to : " << (arg1/(3.1415926*lower.SpoolDiameter))<<endl;
    lower.SetPosition(arg1/(3.1415926* lower.SpoolDiameter));
}
