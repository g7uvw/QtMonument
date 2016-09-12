#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "motor.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool WireSerialOpen;
    bool RotationSerialOpen;


public slots:
    //wires
    void openSerialPort_wire();
    void closeSerialPort_wire();
    void readData_wire();
    void handleError_wire(QSerialPort::SerialPortError error);
    void GetPositions_wire();

    //rotation
    void openSerialPort_rotation();
    void closeSerialPort_rotation();
    void readData_Rotation();
    void handleError_rotation(QSerialPort::SerialPortError error);

    //common



private slots:

    //wires
     void on_actionComms_wire_Settings_triggered();

     //rotation


    //common

    void on_actionConnect_to_motors_triggered();
    void on_action_Exit_triggered();




    void on_lower_speed_spin_valueChanged(int arg1);

    void on_lower_enable_toggled(bool checked);

    void on_upper_enable_toggled(bool checked);

    void on_lower_pos_spin_valueChanged(int arg1);

    void on_upper_diameter_spin_valueChanged(double arg1);

    //void on_lower_enable_clicked(bool checked);

    //void on_lower_enable_clicked();

    void on_Lower_Pos_Zero_clicked();

    void on_EMERGENCY_STOP_clicked();

    void on_upper_speed_spin_editingFinished();

    void on_RUN_clicked();



    void on_lower_diameter_spin_valueChanged(double arg1);



    void on_jog_speed_spin_valueChanged(double arg1);

    void on_jog_speed_save_clicked();

    void on_jog_speed_restore_clicked();

private:
    Ui::MainWindow *ui;
     QSerialPort *serial_wire;
     SettingsDialog *settings_wire;
    Motor upper;
    Motor lower;
    QLabel *serialstatus_wire;
    QLabel *motor1status;
    QLabel *motor2status;
    QTimer *timer;

    bool E_STOPPED;
    double saved_speed;
    double saved_pos;

};

#endif // MAINWINDOW_H
