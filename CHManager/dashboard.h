//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QMainWindow>
#include <QTranslator>

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

    QLabel *curr_device;
    QLabel *curr_datatime;
    QTranslator* translator;

protected:

private slots:
    void getCurrentTime();
    void switchLanguage(QAction* action);

};
#endif // DASHBOARD_H
