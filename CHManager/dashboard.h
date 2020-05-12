//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "device/chserialdevice.h"
#include "console/chconsole.h"
#include "dock/chrealtime.h"

#include <QMainWindow>
#include <QTranslator>
#include <QTimer>

class ToolBar;
class QLabel;
class QGridLayout;
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
    void setupDockWindows();
    void setupDataBinder();
    int getCurrentBaudrate();

    QDockWidget *realdata_dock;
    QDockWidget *heading_dock;
    QDockWidget *attitude_dock;
    QDockWidget *threed_dock;
    QDockWidget *atomspheric_dock;
    QDockWidget *framrate_dock;
    QWidget *centerwidget;
    QGridLayout *grid;
    QLabel *curr_device;
    QLabel *curr_datatime;
    QTranslator* translator;
    CHSerialdevice ch_dev;
    int m_baudrate = 115200;

    CHConsole *console;
    CHRealTime *realdata;

protected:

private slots:
    void getCurrentTime();
    void switchLanguage(QAction* action);
    void openSerialPort(QAction* action);
    void closeSerialPort();
    void setCurrentBaudrate(QAction* action);
    void readData();
    void showConsole();
    void showCustomchart();
    void docktoggleView(bool b);

};
#endif // DASHBOARD_H
