//SPDX-License-Inentifier: Apache-2.0 and GPLv2
/*
 * HiPNUC CHManager Instrument Clusters used to make
 * the Attitude Indicator widget and Compass widget.
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "chinstrumentcluster.h"
#include <math.h>

#include <QTimer>
#include <QPen>
#include <QPainter>

CHAttitudeIndicator::CHAttitudeIndicator(QWidget *parent) : QWidget(parent)
{
    m_sizeMin = 200;
    m_sizeMax = 600;
    m_offset = 2;
    m_size = m_sizeMin - 2*m_offset;

    setMinimumSize(m_sizeMin, m_sizeMin);
    setMaximumSize(m_sizeMax, m_sizeMax);
    resize(m_sizeMin, m_sizeMin);

    setFocusPolicy(Qt::NoFocus);

    m_roll  = 0.0;
    m_pitch = 0.0;

    adi_timer = new QTimer();
    connect(adi_timer, SIGNAL(timeout()), this, SLOT(updateAttitudeIndicator()),  Qt::QueuedConnection);
}

CHAttitudeIndicator::~CHAttitudeIndicator()
{

}

void CHAttitudeIndicator::setData(float roll, float pitch)
{
    m_roll = roll;
    m_pitch = pitch;

    if (m_roll < -180.0f ) m_roll = -180.0f;
    if (m_roll >  180.0f ) m_roll =  180.0f;

    if (m_pitch < -90.0f ) m_pitch = -90.0f;
    if (m_pitch >  90.0f ) m_pitch =  90.0f;

    update();
}

void CHAttitudeIndicator::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_size = qMin(width(),height()) - 2*m_offset;
}


void CHAttitudeIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    QBrush bgSky(QColor(48,172,220));
    QBrush bgGround(QColor(247,168,21));

    QPen   whitePen(Qt::white);
    QPen   blackPen(Qt::black);
    QPen   pitchPen(Qt::white);
    QPen   pitchZero(Qt::green);

    whitePen.setWidth(2);
    blackPen.setWidth(2);
    pitchZero.setWidth(3);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);
    painter.rotate(m_roll);

    double pitch_tem = m_pitch;

    //draw background
    {
        int y_min, y_max;

        y_min = m_size/2*-40.0/45.0;
        y_max = m_size/2* 40.0/45.0;

        int y = m_size/2*pitch_tem/45.;
        if( y < y_min ) y = y_min;
        if( y > y_max ) y = y_max;

        int x = sqrt(m_size*m_size/4 - y*y);
        qreal gr = atan((double)(y)/x);
        gr = gr * 180./3.1415926;

        painter.setPen(blackPen);
        painter.setBrush(bgSky);
        painter.drawChord(-m_size/2, -m_size/2, m_size, m_size,
                          gr*16, (180-2*gr)*16);

        painter.setBrush(bgGround);
        painter.drawChord(-m_size/2, -m_size/2, m_size, m_size,
                          gr*16, -(180+2*gr)*16);
    }

    //set mask
    QRegion maskRegion(-m_size/2, -m_size/2, m_size, m_size, QRegion::Ellipse);
    painter.setClipRegion(maskRegion);

    //draw pitch lines & marker
    {
        int x, y, x1, y1;
        int textWidth;
        double p, r;
        int ll = m_size/8, l;
        int fontSize = 8;
        QString s;

        pitchPen.setWidth(2);
        painter.setFont(QFont("", fontSize));

        // draw lines
        for(int i = -9; i <= 9; i++) {
            p = i * 10;
            s = QString("%1").arg(-p);

            if( i % 3 == 0 )
                l = ll;
            else
                l = ll/2;

            if( i == 0 ) {
                painter.setPen(pitchZero);
                l = l * 1.8;
            } else {
                painter.setPen(pitchPen);
            }

            y = m_size/2*p/45.0 - m_size/2*pitch_tem/45.;
            x = l;

            r = sqrt(x*x + y*y);
            if( r > m_size/2 ) continue;

            painter.drawLine(QPointF(-l, 1.0*y), QPointF(l, 1.0*y));
            textWidth = 100;

            if( i % 3 == 0 && i != 0 ) {
                painter.setPen(QPen(Qt::white));

                x1 = -x - 2 - textWidth;
                y1 = y - fontSize/2 - 1;
                painter.drawText(QRectF(x1, y1, textWidth, fontSize+2),
                                 Qt::AlignRight|Qt::AlignVCenter, s);
            }
        }

        // draw marker
        int     markerSize = m_size/20;
        float   fx1, fy1, fx2, fy2, fx3, fy3;

        painter.setBrush(QBrush(Qt::red));
        painter.setPen(Qt::NoPen);

        fx1 = markerSize;
        fy1 = 0;
        fx2 = fx1 + markerSize;
        fy2 = -markerSize/2;
        fx3 = fx1 + markerSize;
        fy3 = markerSize/2;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);

        QPointF points2[3] = {
            QPointF(-fx1, fy1),
            QPointF(-fx2, fy2),
            QPointF(-fx3, fy3)
        };
        painter.drawPolygon(points2, 3);
    }
    // draw roll degree lines
    {
        int     nRollLines = 36;
        float   rotAng = 360.0 / nRollLines;
        int     rollLineLeng = m_size/25;
        double  fx1, fy1, fx2, fy2;
        int     fontSize = 8;
        QString s;

        blackPen.setWidth(1);
        painter.setPen(blackPen);
        painter.setFont(QFont("", fontSize));

        for(int i=0; i<nRollLines; i++) {
            if( i < nRollLines/2 )
                s = QString("%1").arg(-i*rotAng);
            else
                s = QString("%1").arg(360-i*rotAng);

            fx1 = 0;
            fy1 = -m_size/2 + m_offset;
            fx2 = 0;

            if( i % 3 == 0 ) {
                fy2 = fy1 + rollLineLeng;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));

                fy2 = fy1 + rollLineLeng+2;
                painter.drawText(QRectF(-50, fy2, 100, fontSize+2),
                                 Qt::AlignCenter, s);
            } else {
                fy2 = fy1 + rollLineLeng/2;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));
            }

            painter.rotate(rotAng);
        }
    }
    // draw roll marker
    {
        int     rollMarkerSize = m_size / 25;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        painter.rotate(-m_roll);
        painter.setBrush(QBrush(Qt::black));

        fx1 = 0;
        fy1 = -m_size/2 + m_offset;
        fx2 = fx1 - rollMarkerSize/2;
        fy2 = fy1 + rollMarkerSize;
        fx3 = fx1 + rollMarkerSize/2;
        fy3 = fy1 + rollMarkerSize;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);
    }
}

void CHAttitudeIndicator::updateAttitudeIndicator()
{
    dump_rf_data(&imublock);

    setData(imublock.eul[0], imublock.eul[1]);
}

void CHAttitudeIndicator::transaction()
{
    adi_timer->start(25);
}

CHCompass::CHCompass(QWidget *parent) : QWidget(parent)
{
    m_sizeMin = 200;
    m_sizeMax = 600;
    m_offset = 2;
    m_size = m_sizeMin - 2*m_offset;

    setMinimumSize(m_sizeMin, m_sizeMin);
    setMaximumSize(m_sizeMax, m_sizeMax);
    resize(m_sizeMin, m_sizeMin);

    setFocusPolicy(Qt::NoFocus);

    m_yaw  = 0.0;

    compass_timer = new QTimer();
    connect(compass_timer, SIGNAL(timeout()), this, SLOT(updateCompass()), Qt::QueuedConnection);
}

void CHCompass::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    m_size = qMin(width(),height()) - 2 * m_offset;
}

void CHCompass::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QBrush bgGround(QColor(30,30,30));

    QPen   whitePen(Qt::white);
    QPen   blackPen(Qt::white);

    whitePen.setWidth(2);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);

    // draw background
    {
        painter.setPen(blackPen);
        painter.setBrush(bgGround);

        painter.drawEllipse(-m_size/2, -m_size/2, m_size, m_size);
    }

    // draw yaw lines
    {
        int     nyawLines = 36;
        float   rotAng = 360.0 / nyawLines;
        int     yawLineLeng = m_size/25;
        double  fx1, fy1, fx2, fy2;
        int     fontSize = 8;
        QString s;

        blackPen.setWidth(1);
        painter.setPen(blackPen);

        for(int i=0; i<nyawLines; i++) {

            if( i == 0 ) {
                s = "N";
                painter.setPen(whitePen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 9 ) {
                s = "W";
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 18 ) {
                s = "S";
                painter.setPen(whitePen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 27 ) {
                s = "E";
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else {
                s = QString("%1").arg(i*rotAng);
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize));
            }

            fx1 = 0;
            fy1 = -m_size/2 + m_offset;
            fx2 = 0;

            if( i % 3 == 0 ) {
                fy2 = fy1 + yawLineLeng;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));

                fy2 = fy1 + yawLineLeng+4;
                painter.drawText(QRectF(-50, fy2, 100, fontSize+2),
                                 Qt::AlignCenter, s);
            } else {
                fy2 = fy1 + yawLineLeng/2;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));
            }

            painter.rotate(-rotAng);
        }
    }
    // draw S/N arrow
    {
        int     arrowWidth = m_size/5;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        fx1 = 0;
        fy1 = -m_size/2 + m_offset + m_size/25 + 15;
        fx2 = -arrowWidth/2;
        fy2 = 0;
        fx3 = arrowWidth/2;
        fy3 = 0;

        painter.setPen(Qt::NoPen);

        painter.setBrush(QBrush(Qt::red));
        QPointF pointsN[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(pointsN, 3);


        fx1 = 0;
        fy1 = m_size/2 - m_offset - m_size/25 - 15;
        fx2 = -arrowWidth/2;
        fy2 = 0;
        fx3 = arrowWidth/2;
        fy3 = 0;

        painter.setBrush(QBrush(Qt::blue));
        QPointF pointsS[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(pointsS, 3);
    }

    // draw yaw marker
    {
        int     yawMarkerSize = m_size/12;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        painter.rotate(-m_yaw);
        painter.setBrush(QBrush(QBrush(Qt::red)));

        fx1 = 0;
        fy1 = -m_size/2 + m_offset;
        fx2 = fx1 - yawMarkerSize/2;
        fy2 = fy1 + yawMarkerSize;
        fx3 = fx1 + yawMarkerSize/2;
        fy3 = fy1 + yawMarkerSize;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);

        painter.rotate(m_yaw);
    }
}

void CHCompass::setYaw(float yaw)
{
    m_yaw = yaw;

    if( m_yaw < 0   ) m_yaw = 360 + m_yaw;
    if( m_yaw > 360 ) m_yaw = m_yaw - 360;

    update();
}

void CHCompass::updateCompass()
{
    dump_rf_data(&imublock);

    setYaw(imublock.eul[2]);
}

void CHCompass::transaction()
{
    compass_timer->start(100);
}
