//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "device/chserialdevice.h"
#include "console/chconsole.h"
#include "protocol/kptl.h"
#include "protocol/chhi22x.h"

#include <QMainWindow>
#include <QTranslator>
#include <QTimer>

class ToolBar;
class QLabel;
QT_FORWARD_DECLARE_CLASS(QMenu)

class Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    Dashboard(QWidget *parent = nullptr);
    ~Dashboard();

    void setTranslator(QTranslator* translator);

private:
    void setupMenuBar();
    void setupStatusBar();
    void setupDataBinder();
    int getCurrentBaudrate();

    QLabel *curr_device;
    QLabel *curr_datatime;
    QTranslator* translator;
    CHSerialdevice ch_dev;
    int m_baudrate = 115200;
    imu_data_t imublock;

    CHConsole *console;
    QTimer *imubinder_timer;

protected:

private slots:
    void getCurrentTime();
    void switchLanguage(QAction* action);
    void openSerialPort(QAction* action);
    void closeSerialPort();
    void setCurrentBaudrate(QAction* action);
    void readData();
    void readIMUdata();
    void showConsole();
    void showCustomchart();

};
#endif // DASHBOARD_H
