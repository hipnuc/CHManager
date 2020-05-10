//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef CHSERIALDEVICE_H
#define CHSERIALDEVICE_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class CHSerialdevice : public QObject
{
    Q_OBJECT

public:
    explicit CHSerialdevice(QObject *parent = nullptr);
    void getCurrentPortinfo(int index, QStringList *info, QStringList *state);
    int openSerialPort(QString portname, int baudrate);
    bool  updateBaudRate(int baudrate);
    void closeSerialPort();
    void readData(QByteArray *data, long long *len);
    QSerialPort *m_serial = nullptr;

private:
    int checkPortavailable(QString portname);

};

#endif // CHSERIALDEVICE_H
