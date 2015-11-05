#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "motor.h"

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

private slots:
    void on_actionConnect_to_motors_triggered();
    void on_action_Exit_triggered();


    void on_actionComms_Settings_triggered();

    void on_lower_speed_spin_valueChanged(int arg1);

    void on_lower_enable_toggled(bool checked);

    void on_upper_enable_toggled(bool checked);

    void on_lower_pos_spin_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
     QSerialPort *serial;
     SettingsDialog *settings;
    Motor upper;
    Motor lower;
};

#endif // MAINWINDOW_H
