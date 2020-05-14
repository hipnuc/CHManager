//SPDX-License-Inentifier: Apache-2.0
/*
 * HiPNUC CHManager Real Time View
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "chrealtime.h"

#include <QGroupBox>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>

CHRealTime::CHRealTime(QWidget *parent) : QWidget(parent)
{
    tableWidget = new QTableWidget();
    QGroupBox *group = new QGroupBox();
    QHBoxLayout *layout = new QHBoxLayout();

    tableWidget->setRowCount(10);
    tableWidget->setColumnCount(5);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setVisible(false);

    setFixedHeight(320);

    QString title1="Quaternion,W,X,Y,Z";
    QString title2="Euler Angle,Pitch,Roll,Yaw";
    QString title3="Acceleration,X,Y,Z";
    QString title4="Gyroscope,X,Y,Z";
    QString title5="Magnetic field,X,Y,Z";

    foreach(QString item,  title1.split(",")){
        static int i=0;
        QTableWidgetItem * protoitem = new QTableWidgetItem(item);
        protoitem->setFlags(protoitem->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(0,i,protoitem);
        i++;
    }
    foreach(QString item,  title2.split(",")){
        static int i=0;
        QTableWidgetItem * protoitem = new QTableWidgetItem(item);
        protoitem->setFlags(protoitem->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(2,i,protoitem);
        i++;
    }
    foreach(QString item,  title3.split(",")){
        static int i=0;
        QTableWidgetItem * protoitem = new QTableWidgetItem(item);
        protoitem->setFlags(protoitem->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(4,i,protoitem);
        i++;
    }
    foreach(QString item,  title4.split(",")){
        static int i=0;
        QTableWidgetItem * protoitem = new QTableWidgetItem(item);
        protoitem->setFlags(protoitem->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(6,i,protoitem);
        i++;
    }
    foreach(QString item,  title5.split(",")){
        static int i=0;
        QTableWidgetItem * protoitem = new QTableWidgetItem(item);
        protoitem->setFlags(protoitem->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(8,i,protoitem);
        i++;
    }
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(tableWidget);

    group->setFlat(true);
    group->setFont(QFont("Calibri", 10, QFont::Bold));
    group->setTitle(tr("Real Time Data Preview"));
    group->setStyleSheet("background-color:transparent;background-color:black; color:white;");
    group->setLayout(layout);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(group);

    setLayout(main_layout);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(realtimeDataview()));
}

void CHRealTime::realtimeDataview()
{
    dump_rf_data(&imublock);

    for (int i = 0; i < 4; i++) {
        QString item = QString::number(imublock.q[i], 'f', 3);
        QTableWidgetItem *protoitem = new QTableWidgetItem(item);
        protoitem->setTextAlignment(Qt::AlignCenter);
        protoitem->setTextAlignment(Qt::AlignVCenter);
        tableWidget->setItem(1, i + 1, protoitem);
    }

    for (int i = 0; i < 3; i++) {
        QString item = QString::number(imublock.eul[i], 'f', 3);
        QTableWidgetItem *protoitem = new QTableWidgetItem(item);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(3, i + 1, protoitem);
        if(i == 2)
            tableWidget->setItem(3, 4, new QTableWidgetItem("degree"));
    }

    for (int i = 0; i < 3; i++){
        QString item = QString::number(imublock.acc[i]);
        QTableWidgetItem *protoitem = new QTableWidgetItem(item);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(5, i + 1, protoitem);
        if(i == 2)
            tableWidget->setItem(5, 4, new QTableWidgetItem("0.001G"));
    }

    for (int i = 0; i < 3; i++){
        QString item = QString::number(imublock.gyr[i]);
        QTableWidgetItem *protoitem = new QTableWidgetItem(item);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(7, i + 1, protoitem);
        if(i == 2)
            tableWidget->setItem(7, 4, new QTableWidgetItem("0.1°/s"));
    }

    for (int i = 0; i < 3; i++){
        QString item = QString::number(imublock.mag[i]);
        QTableWidgetItem * protoitem = new QTableWidgetItem(item);
        protoitem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(9, i + 1, protoitem);
        if(i == 2)
            tableWidget->setItem(9, 4, new QTableWidgetItem("0.001Gauss"));
    }
}

void CHRealTime::transaction()
{
    timer->start(10);
}

void CHRealTime::abort()
{
    timer->stop();
}
