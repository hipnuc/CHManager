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

    m_console = new QPlainTextEdit();
    m_console->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_console->centerOnScroll();
    m_console->setReadOnly(true);

    QTextCursor cursor = m_console->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_console->setTextCursor(cursor);

    setupLayout();
    setupBinder();
}

void CHConsole::setupLayout()
{
    lock_btn = new QPushButton();
    clear_btn = new QPushButton();
    timestamp_btn =  new QPushButton();
    filter_btn = new QPushButton();
    filter_text = new QLineEdit();

    lock_btn->setIcon(QIcon(":/images/resource/padlock_32px.png"));
    clear_btn->setIcon(QIcon(":/images/resource/broom_32px.png"));
    timestamp_btn->setIcon(QIcon(":/images/resource/timesheet_32px.png"));
    filter_btn->setIcon(QIcon(":/images/resource/filter_32px.png"));

    lock_btn->setToolTip(tr("Lock Update"));
    clear_btn->setToolTip(tr("Clear Text Window"));
    timestamp_btn->setToolTip(tr("Show/Hide PC Time"));
    filter_btn->setToolTip(tr("Enable or disable the filter"));

    lock_btn->setAutoExclusive(true);
    lock_btn->setCheckable(true);
    clear_btn->setCheckable(true);
    timestamp_btn->setCheckable(true);
    filter_btn->setCheckable(true);

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

void CHConsole::setupBinder()
{
     timer = new QTimer();
     connect(timer, SIGNAL(timeout()), this, SLOT(formatPackets()));
     connect(clear_btn, SIGNAL(clicked()), this, SLOT(clearTextEdit()));
     connect(lock_btn, SIGNAL(clicked()), this, SLOT(lockTextEdit()));
     connect(filter_btn, SIGNAL(clicked()), this, SLOT(filterPackets()));
     connect(timestamp_btn, SIGNAL(clicked()), this, SLOT(showPCtimestamp()));
}

void CHConsole::updateTextdata(QByteArray array)
{
    m_console->insertPlainText(array.toHex());
}

void CHConsole::clearTextEdit()
{
   this->m_console->clear();
}

void CHConsole::lockTextEdit()
{
    if (lock_btn->isChecked()) {
        lock_btn->setChecked(true);
        lock_btn->setIcon(QIcon(":/images/resource/lock_32px.png"));
        timer->stop();
    }else {
        lock_btn->setChecked(false);
        lock_btn->setIcon(QIcon(":/images/resource/padlock_32px.png"));
        timer->start(1000);
    }
}

void CHConsole::formatPackets()
{
    //Fake messsages
    QString format_tile = "??:??:?? ";
    QString format_time;
    QString format_context = "R -> NMEA GNGLL,  Size  20,  'Geographic Position - Latitude/Longitude'";
    int j = 0;
    bool flags = false;

    dump_rf_data(&imublock);
    //format = QString("%1").arg(imublock.eul[0]);

    if (pc_time_flags)
        format_time = QDateTime::currentDateTime().toString("[hh:mm:ss:zzz] ");

    if (filter_flags) {
        /**
         * Filter the packects if the regular is true
         */
        while((j = format_context.indexOf(filter_text->text(), j)) != -1) {
            flags = true;
            j++;
        }

        if (flags)
            this->m_console->insertPlainText(format_tile + format_time + format_context + "\n");
    }else
        this->m_console->insertPlainText(format_tile + format_time + format_context + "\n");
}

void CHConsole::transaction()
{
    timer->start(1000);
}

void CHConsole::filterPackets()
{
    if (filter_btn->isChecked()) {
        this->filter_flags = true;
        lock_btn->setChecked(true);
    }else {
        this->filter_flags = false;
        lock_btn->setChecked(false);
    }
}

void CHConsole::showPCtimestamp()
{
    if (timestamp_btn->isChecked()) {
        this->pc_time_flags = true;
        lock_btn->setChecked(true);
    }else {
        this->pc_time_flags = false;
        timestamp_btn->setChecked(false);
    }
}
