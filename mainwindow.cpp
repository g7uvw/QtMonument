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

    Motor lower(serial,1);
    Motor upper(serial,2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionConnect_to_motors_triggered()
{

        openSerialPort();
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

}
