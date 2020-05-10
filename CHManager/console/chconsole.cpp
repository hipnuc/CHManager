//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "chconsole.h"

#include <QtWidgets>

CHConsole::CHConsole(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(600, 400);

    setupLayout();
}

void CHConsole::setupLayout()
{
    m_console = new QPlainTextEdit();
    m_console->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_console->centerOnScroll();

    lock_btn = new QPushButton();
    clear_btn = new QPushButton();
    timestamp_btn =  new QPushButton();
    filter_btn = new QPushButton();
    filter_text = new QLineEdit();
    lock_btn->setIcon(QIcon(":/images/resource/lock_32px.png"));
    clear_btn->setIcon(QIcon(":/images/resource/broom_32px.png"));
    timestamp_btn->setIcon(QIcon(":/images/resource/timesheet_32px.png"));
    filter_btn->setIcon(QIcon(":/images/resource/filter_32px.png"));
    lock_btn->setToolTip(tr("Lock Update"));
    clear_btn->setToolTip(tr("Clear Text Window"));
    timestamp_btn->setToolTip(tr("Show/Hide PC Time"));
    filter_btn->setToolTip(tr("Enable or disable the filter"));

    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->addWidget(lock_btn);
    h_layout->addWidget(clear_btn);
    h_layout->addWidget(timestamp_btn);
    h_layout->addWidget(filter_btn);
    h_layout->addWidget(filter_text);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_console);
    layout->addLayout(h_layout);

    this->setLayout(layout);
}

void CHConsole::updateTextdata(QByteArray array)
{
    QTextCursor cursor = m_console->textCursor();
    cursor.movePosition(QTextCursor::End);

    m_console->setTextCursor(cursor);
    m_console->insertPlainText(array.toHex());
}
