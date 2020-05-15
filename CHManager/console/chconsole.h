//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef CHCONSOLE_H
#define CHCONSOLE_H

#include <QWidget>
#include "protocol/kptl.h"
#include "protocol/chhi22x.h"

class QLabel;
class QPushButton;
class QPlainTextEdit;
class QLineEdit;

class CHConsole : public QWidget
{
    Q_OBJECT
public:
    explicit CHConsole(QWidget *parent = nullptr);
    void updateTextdata(QByteArray array);
    void transaction();

private:
    QPlainTextEdit *m_console;
    QPushButton *lock_btn;
    QPushButton *clear_btn;
    QPushButton *timestamp_btn;
    QPushButton *filter_btn;
    QLineEdit *filter_text;
    QTimer *timer;
    id0x91_t imublock;
    bool filter_flags = false;
    bool pc_time_flags = false;

    void setupLayout();
    void setupBinder();

private slots:
    void clearTextEdit();
    void lockTextEdit();
    void formatPackets();
    void filterPackets();
    void showPCtimestamp();
};

#endif // CHCONSOLE_H
