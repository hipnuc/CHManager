//SPDX-License-Inentifier: Apache-2.0
/*
 * HiPNUC Custom Plots interface used to show a Real Time
 * Chart view.
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef CHCUSTOMPLOT_H
#define CHCUSTOMPLOT_H

#include "qcustomplot/qcustomplot.h"
#include "../protocol/chhi22x.h"
#include "../protocol/kptl.h"

#include <QWidget>

class CHCustomPlot : public QWidget
{
    Q_OBJECT
public:
    explicit CHCustomPlot(const int type, QWidget *parent = nullptr);
    ~CHCustomPlot();

    void transaction();
    void abort();

private:
    QCustomPlot *customPlot;
    QTimer *timer, *plottimer;
    QTimer curr_timer;
    bool plotState;
    double runTimes;

    int type = 0xff;

    id0x91_t  imublock;

    QGridLayout *layout;
    QHBoxLayout *menu_layout;

    QToolButton *screenshot_btn, *clear_btn;
    QToolButton *setting_btn, *line_btn;
    QToolButton *lock_btn;

    QString curr_time;

    void setupCustomplot(const QString &title);
    void configType(const int type);
    void setupMenu();
    void setupLayout();
    void setSignal(QCustomPlot *customPlot, int index, double data, double xAxis);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void sendEvent(const int type);

private slots:
    void updateCustomPlot();
    void updateData();
    void ButtonPressEvent();
    void lockChartEvent();
    void getCurrentTime();
};

#endif // CHCUSTOMPLOT_H
