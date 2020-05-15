//SPDX-License-Inentifier: Apache-2.0 and GPLv2
/*
 * HiPNUC CHManager Instrument Clusters used to make
 * the Attitude Indicator widget and Compass widget.
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef CHINSTRUMENTCLUSTER_H
#define CHINSTRUMENTCLUSTER_H

#include <QWidget>

#include "protocol/chhi22x.h"
#include "protocol/kptl.h"

/**
 * @brief The Attitude Indicator class
 */
class CHAttitudeIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit CHAttitudeIndicator(QWidget *parent = nullptr);
    ~CHAttitudeIndicator();
    void transaction();
    void setData(float roll, float pitch);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    int     m_sizeMin, m_sizeMax;
    int     m_size, m_offset;

    //Roll and Pitch angle (in degree)
    float m_roll;
    float m_pitch;
    QTimer *adi_timer;
    id0x91_t imublock;

private slots:
    void updateAttitudeIndicator();
};

/**
 * @brief The CHCompass class
 */
class CHCompass : public QWidget
{
    Q_OBJECT
public:
    explicit CHCompass(QWidget *parent = nullptr);
    void setYaw(float yaw);
    void transaction();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    int     m_sizeMin, m_sizeMax;
    int     m_size, m_offset;

    //Yaw angle (in degree)
    double  m_yaw;
    QTimer *compass_timer;
    id0x91_t imublock;

private slots:
    void updateCompass();
};

#endif // CHINSTRUMENTCLUSTER_H
