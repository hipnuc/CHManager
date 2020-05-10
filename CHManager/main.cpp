//SPDX-License-Inentifier: Apache-2.0
/*
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "dashboard.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(QString(":/languages/CHManager_zh_CN"));
    a.installTranslator(&translator);

    Dashboard w;
    w.setTranslator(&translator);
    w.show();
    return a.exec();
}
