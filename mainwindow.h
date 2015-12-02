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

    bool SerialOpen;

public slots:
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void openSerialPort();
    void closeSerialPort();
    void GetPositions();

private slots:
    void on_actionConnect_to_motors_triggered();
    void on_action_Exit_triggered();


    void on_actionComms_Settings_triggered();

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

private:
    Ui::MainWindow *ui;
     QSerialPort *serial;
     SettingsDialog *settings;
    Motor upper;
    Motor lower;
    QLabel *serialstatus;
    QLabel *motor1status;
    QLabel *motor2status;
    QTimer *timer;

    bool E_STOPPED;

};

#endif // MAINWINDOW_H
