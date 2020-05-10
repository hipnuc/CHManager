//SPDX-License-Inentifier: Apache-2.0
/*
 * HiPNUC Serial IMU device handle that supports HI221/226/229
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "chserialdevice.h"

static const char blankString[] = QT_TRANSLATE_NOOP("CHSerialdevice", "N/A");

CHSerialdevice::CHSerialdevice(QObject *parent) : QObject(parent)
{
    m_serial = new QSerialPort();
}

/**
 * @brief CHSerialdevice::checkPortavailable
 * @param portname: serial port number
 * @return: 0
 */
int CHSerialdevice::checkPortavailable(QString portname)
{
    int ret;

    ret = this->openSerialPort(portname, 115200);
    this->closeSerialPort();

    return ret;
}

/**
 * @brief CHSerialdevice::getCurrentPortinfo - get Current system avaiable Serial Port information
 * @param index: index of the serial port requirement.
 * @param list: QStringList pointer used as ports
 * @param state: QStringList pointer used as available port state
 */
void CHSerialdevice::getCurrentPortinfo(int index, QStringList *list, QStringList *state)
{
    QString description;
    QString manufacturer;
    QString serialNumber;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();

        switch(index) {
        case 0:
            list->append(info.portName());
            if (checkPortavailable(info.portName()))
                state->append("0");
            else
                state->append("1");
            break;
        case 1:
            *list << info.portName()
                 << (!description.isEmpty() ? description : blankString)
                 << (!manufacturer.isEmpty() ? manufacturer : blankString)
                 << (!serialNumber.isEmpty() ? serialNumber : blankString)
                 << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
                 << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);
            break;
        default:
            break;
        }
    }
}

/**
 * @brief CHSerialdevice::updateBaudRate - Change the BaudRate for selected Serial Port
 * @param baudrate
 * @return
 */
bool CHSerialdevice::updateBaudRate(int baudrate)
{
    return this->m_serial->setBaudRate(baudrate);
}

/**
 * @brief CHSerialdevice::openSerialPort - Open an available Serial Port
 * @param portname
 * @param baudrate
 * @return
 */
int CHSerialdevice::openSerialPort(QString portname, int baudrate)
{
    m_serial->setPortName(portname);
    m_serial->setBaudRate(baudrate);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial->open(QIODevice::ReadWrite)) {
        return -1;
    }

    return 0;
}

/**
 * @brief CHSerialdevice::closeSerialPort
 */
void CHSerialdevice::closeSerialPort()
{
    if(this->m_serial->isOpen()) {
        this->m_serial->disconnect();
        this->m_serial->close();
    }
}

/**
 * @brief CHSerialdevice::readData
 * @param data
 */
void CHSerialdevice::readData(QByteArray *array, long long *length)
{
    auto len = this->m_serial->bytesAvailable();

    if (len > 0 && this->m_serial->isReadable()) {
        array->append(this->m_serial->readAll());
    }

    *length = len;
}
