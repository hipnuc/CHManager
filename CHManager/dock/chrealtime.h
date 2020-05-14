//SPDX-License-Inentifier: Apache-2.0
/*
 * HiPNUC CHManager Real Time View
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef CHREALTIME_H
#define CHREALTIME_H

#include <QWidget>
#include "protocol/chhi22x.h"
#include "protocol/kptl.h"

class QLabel;
class QTableWidget;

class CHRealTime : public QWidget
{
    Q_OBJECT
public:
    explicit CHRealTime(QWidget *parent = nullptr);
    void transaction();
    void abort();

private:
    QLabel *content;
    QLabel *statistical;
    QTimer *timer;
    QTableWidget *tableWidget;
    imu_data_t imublock;

private slots:
    void realtimeDataview();
};

#endif // CHREALTIME_H
