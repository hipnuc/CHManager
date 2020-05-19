//SPDX-License-Inentifier: Apache-2.0
/*
 * HiPNUC Custom Plots interface used to show a Real Time
 * Chart view.
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "chcustomplot.h"

CHCustomPlot::CHCustomPlot(const int type, QWidget *parent) : QWidget(parent)
{
    this->resize(960, 540);

    configType(type);
    setupLayout();
}

void CHCustomPlot::configType(const int type)
{
    QString plot_title;

    switch (type) {
        case typeGyroscope:
        plot_title.append("Gyroscope (°/s)");
        break;
    case typeAcceleration:
        plot_title.append("Acceleration (G)");
        break;
    case typeMagnetic:
        plot_title.append("Magnetic field (uT)");
        break;
    case typeEuler:
        plot_title.append("Euler Angle (°)");
        break;
    default:
        break;
    }

    this->type = type;
    this->setWindowTitle("CHManager - " + plot_title);

    setupMenu();
    setupCustomplot(plot_title);
}

void CHCustomPlot::setupCustomplot(const QString &title)
{
    this->plotState = false;
    this->runTimes  = 0;

    const QColor colorTable[3] = {
        Qt::red,      Qt::green,    Qt::blue
    };

    customPlot = new QCustomPlot(this);
    customPlot->setObjectName(title.mid(0, 3));
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectPlottables);
    customPlot->xAxis->setLabel("Current Time");
    customPlot->xAxis->setRange(0, 50);
    customPlot->yAxis->setLabel(title.trimmed());
    customPlot->yAxis->setRange(-10, 10);
    customPlot->axisRect()->setupFullAxesBox();

    for(uint8_t i = 0; i < 3; i++) {
        customPlot->addGraph();
        customPlot->graph(i)->setPen(QPen(colorTable[i]));
    }

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setAutoFillBackground(false);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));

    plottimer = new QTimer();
    connect(plottimer, SIGNAL(timeout()), this, SLOT(updateCustomPlot()));
}

CHCustomPlot::~CHCustomPlot()
{

}

void CHCustomPlot::setupMenu()
{
    lock_btn = new QToolButton();
    lock_btn->setIcon(QIcon(":/images/resource/padlock_32px.png"));
    lock_btn->setStatusTip(tr("Lock current Chart"));
    lock_btn->setAutoExclusive(true);
    lock_btn->setCheckable(true);

    screenshot_btn = new QToolButton();
    screenshot_btn->setObjectName("save_screen");
    screenshot_btn->setIcon(QIcon(":/images/resource/screenshot_32px.png"));
    screenshot_btn->setStatusTip(tr("Screenshot current View Chart"));

    clear_btn = new QToolButton();
    clear_btn->setObjectName("clear_plot");
    clear_btn->setIcon(QIcon(":/images/resource/erase_32px.png"));
    clear_btn->setStatusTip(tr("Clear current workspace View Chart"));

    setting_btn = new QToolButton();
    setting_btn->setIcon(QIcon(":/images/resource/settings_32px.png"));
    setting_btn->setStatusTip(tr("Chart Setting"));

    line_btn = new QToolButton();
    line_btn->setIcon(QIcon(":/images/resource/line_width_32px.png"));
    line_btn->setStatusTip(tr("Change the Specify View Chart Lines option"));

    connect(lock_btn, SIGNAL(clicked()), this, SLOT(lockChartEvent()));
    connect(screenshot_btn, SIGNAL(clicked()), this, SLOT(ButtonPressEvent()));
    connect(clear_btn, SIGNAL(clicked()), this, SLOT(ButtonPressEvent()));

    connect(&curr_timer, SIGNAL(timeout()), this, SLOT(getCurrentTime()));
    curr_timer.start(10);
}

void CHCustomPlot::setupLayout()
{
    menu_layout = new QHBoxLayout();
    menu_layout->addWidget(lock_btn);
    menu_layout->addWidget(screenshot_btn);
    menu_layout->addWidget(clear_btn);
    menu_layout->addWidget(setting_btn);
    menu_layout->addWidget(line_btn);
    menu_layout->addStretch();

    layout = new QGridLayout(this);
    layout->addLayout(menu_layout, 0, 0);
    layout->addWidget(customPlot, 1, 0, 1, 1);

    setLayout(layout);
}

void CHCustomPlot::setSignal(QCustomPlot *customPlot, int index, double data, double xAxis)
{
    customPlot->graph(index)->addData(xAxis, data);
}

void CHCustomPlot::updateCustomPlot()
{
    if(this->plotState == true) {
        customPlot->xAxis->setRange(this->runTimes, 50, Qt::AlignRight);
        customPlot->replot();
        this->plotState = false;
    }
}

void CHCustomPlot::updateData()
{
    dump_rf_data(&imublock);
    this->runTimes++;

    for (int i = 0; i < 3; i++) {
        if (this->type == typeGyroscope)
            this->setSignal(customPlot, i, imublock.gyr[i], this->runTimes);
        else if (this->type == typeAcceleration)
            this->setSignal(customPlot, i, imublock.acc[i], this->runTimes);
        else if (this->type == typeMagnetic)
            this->setSignal(customPlot, i, imublock.mag[i], this->runTimes);
        else if (this->type == typeEuler)
            this->setSignal(customPlot, i, imublock.eul[i], this->runTimes);
    }

    this->plotState = true;
}

void CHCustomPlot::ButtonPressEvent()
{
    QToolButton *button =  qobject_cast <QToolButton *>(sender());

    if (button->objectName() == "save_screen") {
        customPlot->savePng(customPlot->objectName() + curr_time + ".png", customPlot->width(), customPlot->height());
        QDesktopServices::openUrl(QDir::currentPath());
    } else if (button->objectName() == "clear_plot") {
        for (int i = 0; i < 3; i++)
        customPlot->graph(i)->clearData();
    }
}

void CHCustomPlot::lockChartEvent()
{
    if (lock_btn->isChecked()) {
        lock_btn->setChecked(true);
        lock_btn->setIcon(QIcon(":/images/resource/lock_32px.png"));
        this->abort();
    }else {
        lock_btn->setChecked(false);
        lock_btn->setIcon(QIcon(":/images/resource/padlock_32px.png"));
        this->transaction();
    }
}

void CHCustomPlot::getCurrentTime()
{
    QDateTime now = QDateTime::currentDateTime();
    QString time = now.toString("yyyy/MM/dd hh:mm:ss");
    QString curr_time = now.toString("yyyy-MM-dd-hh-mm-ss");

    this->curr_time = curr_time;
    customPlot->xAxis->setLabel("Current Time:" + time);
}

void CHCustomPlot::closeEvent(QCloseEvent *event)
{
    event->accept();

    emit sendEvent(this->type);
}

void CHCustomPlot::transaction()
{
    timer->start(100);
    plottimer->start(0);
}

void CHCustomPlot::abort()
{
    timer->stop();
    plottimer->stop();
}
