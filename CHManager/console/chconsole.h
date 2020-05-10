//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#ifndef CHCONSOLE_H
#define CHCONSOLE_H

#include <QWidget>

class QPushButton;
class QPlainTextEdit;
class QLineEdit;

class CHConsole : public QWidget
{
    Q_OBJECT
public:
    explicit CHConsole(QWidget *parent = nullptr);
    void updateTextdata(QByteArray array);

private:
    QPlainTextEdit *m_console;
    QPushButton *lock_btn;
    QPushButton *clear_btn;
    QPushButton *timestamp_btn;
    QPushButton *filter_btn;
    QLineEdit *filter_text;

    void setupLayout();
};

#endif // CHCONSOLE_H
