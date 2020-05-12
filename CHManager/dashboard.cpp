//SPDX-License-Inentifier: Apache-2.0
/*
 * HiPNUC CHManager Dashboard View Layout
 *
 * Copyright (C) 2016-2020 HiPNUC All rights reserved.
 *
 */

#include "dashboard.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QEvent>
#include <QApplication>
#include <QDockWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QDebug>

Dashboard::Dashboard(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName("Dashboard");
    setWindowTitle(tr("CHManager Dashboard"));
    setMinimumSize(1280, 720);

//! Center Widget
    centerwidget = new QWidget();
    setCentralWidget(centerwidget);

    grid = new QGridLayout();
    centerwidget->setLayout(grid);

    setupMenuBar();
    setupStatusBar();
    setupDockWindows();
    setupDataBinder();
}

Dashboard::~Dashboard()
{
}

void Dashboard::setupDataBinder()
{
    console = new CHConsole();
    realdata = new CHRealTime();

    realdata_dock->setWidget(realdata);
    realdata->transaction();

    connect(ch_dev.m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Dashboard::setupMenuBar()
{

    //! [Device Menu]
        QMenu *deviceMenu = menuBar()->addMenu(tr("&Device(D)"));
        QToolBar *deviceToolBar = addToolBar(tr("Device"));

        QMenu *connectionMenu = new QMenu(tr("Connection"));
        const QIcon connection_Icon = QIcon::fromTheme("", QIcon(":/images/resource/disconnected_32px.png"));
        connectionMenu->setIcon(connection_Icon);
        connectionMenu->setStatusTip(tr("Select the Serial Port"));

        const QIcon disconnected_Icon = QIcon::fromTheme("", QIcon(":/images/resource/disconnected_32px.png"));
        QAction *action = connectionMenu->addAction(disconnected_Icon, tr("Disconnect"));
        action->setCheckable(true);
        action->setStatusTip(tr("Disconnect Serial Port"));
        connect(action, SIGNAL(triggered()), this, SLOT(closeSerialPort()));

        action = connectionMenu->addSeparator();

        const QIcon refresh_Icon = QIcon::fromTheme("", QIcon(":/images/resource/synchronize_32px.png"));
        action = connectionMenu->addAction(refresh_Icon, tr("Refresh"));
        action->setCheckable(true);
        action->setStatusTip(tr("Refresh Serial Ports"));
        action = connectionMenu->addSeparator();

        QActionGroup *binderport = new QActionGroup(this);
        QList <QAction *> actionports;
        QStringList portlist, flags;

        ch_dev.getCurrentPortinfo(0, &portlist, &flags);

        for (int i = 0; i < portlist.size(); i++) {
            actionports += new QAction(portlist[i], binderport);
            actionports[i]->setCheckable(true);
            if (!flags[i].toInt())
                actionports[i]->setDisabled(true);
        }
        connectionMenu->addActions(actionports);
        deviceMenu->addMenu(connectionMenu);
        deviceToolBar->addAction(connectionMenu->menuAction());
        connect(binderport, SIGNAL(triggered(QAction *)), this, SLOT(openSerialPort(QAction* )));

        QMenu *baudrateMenu = new QMenu(tr("Baudrate"));
        const QIcon arrow_Icon = QIcon::fromTheme("", QIcon(":/images/resource/single_choice_32px.png"));
        QList <QAction *> actionbaud;
        QActionGroup *binderbaud = new QActionGroup(this);
        QStringList lists = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600", "custom"};
        for(int i = 0; i < lists.size(); i++) {
            actionbaud += new QAction(lists[i], binderbaud);
            actionbaud[i]->setCheckable(true);
        }
        baudrateMenu->setIcon(arrow_Icon);
        actionbaud[7]->setChecked(true);
        baudrateMenu->addActions(actionbaud);
        deviceMenu->addMenu(baudrateMenu);
        deviceMenu->addSeparator();
        deviceToolBar->addAction(baudrateMenu->menuAction());
        deviceToolBar->addSeparator();
        connect(binderbaud, SIGNAL(triggered(QAction *)), this, SLOT(setCurrentBaudrate(QAction* )));

        const QIcon upgradeFirmware_Icon = QIcon::fromTheme("", QIcon(":/images/resource/upgrade_32px.png"));
        action = deviceMenu->addAction(upgradeFirmware_Icon, tr("Firmware Update"));
        action->setStatusTip(tr("Updates the Firmware"));
        deviceMenu->addAction(action);
        deviceToolBar->addAction(action);
        deviceToolBar->addSeparator();
        deviceMenu->addSeparator();

        QMenu *languageMenu = new QMenu(tr("Language"));
        const QIcon geography_Icon = QIcon::fromTheme("", QIcon(":/images/resource/geography_32px.png"));
        languageMenu->setIcon(geography_Icon);

        QActionGroup* langGroup = new QActionGroup(languageMenu);
        langGroup->setExclusive(true);

        const QIcon china_Icon = QIcon::fromTheme("", QIcon(":/images/resource/china_32px.png"));
        const QIcon usa_Icon = QIcon::fromTheme("", QIcon(":/images/resource/usa_32px.png"));
        action = languageMenu->addAction(china_Icon, tr("zh_CN"));
        action->setCheckable(true);
        action->setChecked(true);
        action->setStatusTip(tr("Change the Language to Chinese Simple"));
        langGroup->addAction(action);

        action = languageMenu->addAction(usa_Icon, tr("en_US"));
        action->setCheckable(true);
        action->setStatusTip(tr("Change the Language to English"));
        deviceMenu->addMenu(languageMenu);
        deviceMenu->addSeparator();
        deviceToolBar->addSeparator();
        deviceToolBar->addAction(languageMenu->menuAction());
        langGroup->addAction(action);

        connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(switchLanguage(QAction *)));

        const QIcon assistant_Icon = QIcon::fromTheme("", QIcon(":/images/resource/usb_micro_b_32px.png"));
        action = new QAction();
        action->setCheckable(true);
        action->setIcon(assistant_Icon);
        action->setStatusTip(tr("Serial Assistant"));
        action->setStatusTip(tr("Serial Assistant"));
        deviceToolBar->addAction(action);

        const QIcon exit_Icon = QIcon::fromTheme("", QIcon(":/images/resource/shutdown_32px.png"));
        deviceMenu->addAction(exit_Icon, tr("Exit"), this, &QWidget::close);

    //! [Edit Menu]
        QMenu *editMenu = menuBar()->addMenu(tr("Edit(E)"));
        QToolBar *editToolBar = addToolBar(tr("Edit"));
        addToolBarBreak(Qt::TopToolBarArea);

#ifndef QT_NO_CLIPBOARD
        const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/resource/cut_32px.png"));
        action = editMenu->addAction(cutIcon, tr("Cut"));
        action->setShortcuts(QKeySequence::Cut);
        action->setStatusTip(tr("Cut the current selection's contents to the "
                                "clipboard"));
    //    connect(action, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
        editToolBar->addAction(action);

        const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/resource/copy_32px.png"));
        action = editMenu->addAction(copyIcon, tr("&Copy"));
        action->setShortcuts(QKeySequence::Copy);
        action->setStatusTip(tr("Copy the current selection's contents to the "
                                 "clipboard"));
     //   connect(action, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
        editToolBar->addAction(action);

        const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/resource/paste_32px.png"));
        action = editMenu->addAction(copyIcon, tr("&Paste"));
        action->setShortcuts(QKeySequence::Paste);
        action->setStatusTip(tr("Paste the clipboard's contents into the current "
                                  "selection"));
    //    connect(action, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
        editToolBar->addAction(action);

#endif // !QT_NO_CLIPBOARD

    //! [View Menu]
       QMenu *viewMenu = menuBar()->addMenu(tr("View(V)"));
       QMenu *chartMenu = new QMenu(tr("Chart View"));
       QMenu *dockWidgetMenu = new QMenu(tr("Docking Windows"));
       QToolBar *viewToolBar = addToolBar(tr("View"));
       addToolBarBreak(Qt::TopToolBarArea);

       const QIcon messageView_Icon = QIcon::fromTheme("", QIcon(":/images/resource/message_promotion_32px.png"));
       action = viewMenu->addAction(messageView_Icon, tr("Message View"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Message View"));
       viewToolBar->addAction(action);

       const QIcon packageConsole_Icon = QIcon::fromTheme("", QIcon(":/images/resource/text_32px.png"));
       action = viewMenu->addAction(packageConsole_Icon, tr("Package Console"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Package Console"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered()), this, SLOT(showConsole()));

       const QIcon textConsole_Icon = QIcon::fromTheme("", QIcon(":/images/resource/text_32px.png"));
       action = viewMenu->addAction(textConsole_Icon, tr("Text Console"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Text Console"));
       viewToolBar->addAction(action);

       const QIcon binaryConsole_Icon = QIcon::fromTheme("", QIcon(":/images/resource/binary_file_32px.png"));
       action = viewMenu->addAction(binaryConsole_Icon, tr("Binary Console"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Binary Console"));
       viewToolBar->addAction(action);

       action = viewMenu->addSeparator();

       const QIcon gyroscope_Icon = QIcon::fromTheme("", QIcon(":/images/resource/gyroscope_32px.png"));
       action = chartMenu->addAction(gyroscope_Icon, tr("Gyroscope"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Chart View with Gyroscope"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered()), this, SLOT(showCustomchart()));

       const QIcon acceleration_Icon = QIcon::fromTheme("", QIcon(":/images/resource/speedometer_32px.png"));
       action = chartMenu->addAction(acceleration_Icon, tr("Acceleration"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Chart View with Acceleration"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered()), this, SLOT(showCustomchart()));

       const QIcon magnetic_field_Icon = QIcon::fromTheme("", QIcon(":/images/resource/magnetic_32px.png"));
       action = chartMenu->addAction(magnetic_field_Icon, tr("Magnetic Field"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Chart View with Magnetic Field"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered()), this, SLOT(showCustomchart()));

       const QIcon euler_Icon = QIcon::fromTheme("", QIcon(":/images/resource/line_chart_32px.png"));
       action = chartMenu->addAction(euler_Icon, tr("Euler Angle"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Chart View with Euler Angle"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered()), this, SLOT(showCustomchart()));

       const QIcon quaternion_Icon = QIcon::fromTheme("", QIcon(":/images/resource/quaterly_32px.png"));
       action = chartMenu->addAction(quaternion_Icon, tr("Quaternion"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Chart View with Quaternion"));
       viewToolBar->addAction(action);
       viewMenu->addMenu(chartMenu);
       viewMenu->addSeparator();
       connect(action, SIGNAL(triggered()), this, SLOT(showCustomchart()));

       const QIcon tracker_Icon = QIcon::fromTheme("", QIcon(":/images/resource/track_order_32px.png"));
       action = chartMenu->addAction(tracker_Icon, tr("UWB Tracker"));
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a UWB Tracker View with UWB"));
       viewToolBar->addAction(action);

       const QIcon data_Icon = QIcon::fromTheme("", QIcon(":/images/resource/realdata_30px.png"));
       action = dockWidgetMenu->addAction(data_Icon, tr("Real Time Data"));
       action->setCheckable(true);
       action->setChecked(true);
       action->setObjectName("act_realtime");
       action->setStatusTip(tr("Show current Real Time valid data View"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered(bool)), this, SLOT(docktoggleView(bool)));

       const QIcon heading_Icon = QIcon::fromTheme("", QIcon(":/images/resource/airport_32px.png"));
       action = dockWidgetMenu->addAction(heading_Icon, tr("Heading"));
       action->setObjectName("act_heading");
       action->setCheckable(true);
       action->setStatusTip(tr("Creates a Heading View with Euler Angle"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered(bool)), this, SLOT(docktoggleView(bool)));

       const QIcon attitude_Icon = QIcon::fromTheme("", QIcon(":/images/resource/attitude_32px.png"));
       action = dockWidgetMenu->addAction(attitude_Icon, tr("Attitude"));
       action->setObjectName("act_attitude");
       action->setCheckable(true);
       action->setStatusTip(tr("Show a Attitude View"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered(bool)), this, SLOT(docktoggleView(bool)));

       const QIcon threedimensional_Icon = QIcon::fromTheme("", QIcon(":/images/resource/3d_32px.png"));
       action = dockWidgetMenu->addAction(threedimensional_Icon, tr("3D Attitude"));
       action->setObjectName("act_threed");
       action->setCheckable(true);
       action->setStatusTip(tr("Show a 3D Attitude View"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered(bool)), this, SLOT(docktoggleView(bool)));

       const QIcon atomspheric_Icon = QIcon::fromTheme("", QIcon(":/images/resource/atmospheric_pressure_32px.png"));
       action = dockWidgetMenu->addAction(atomspheric_Icon, tr("Atomspheric"));
       action->setObjectName("act_atomspheric");
       action->setCheckable(true);
       action->setStatusTip(tr("Show a current Atomspheric View"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered(bool)), this, SLOT(docktoggleView(bool)));

       const QIcon framerate_Icon = QIcon::fromTheme("", QIcon(":/images/resource/frame_rate_50px.png"));
       action = dockWidgetMenu->addAction(framerate_Icon, tr("Frame Rate"));
       action->setObjectName("act_framrate");
       action->setCheckable(true);
       action->setStatusTip(tr("Show a current Frame Rate of Device"));
       viewToolBar->addAction(action);
       connect(action, SIGNAL(triggered(bool)), this, SLOT(docktoggleView(bool)));

       viewMenu->addMenu(dockWidgetMenu);

   //! [Player View]
       QMenu *playerMenu = menuBar()->addMenu(tr("Player(P)"));
       QToolBar *playerToolBar = addToolBar(tr("Player"));
       addToolBarBreak(Qt::TopToolBarArea);

       const QIcon eject_Icon = QIcon::fromTheme("", QIcon(":/images/resource/eject_32px.png"));
       action = playerMenu->addAction(eject_Icon, tr("Eject"));
       action->setCheckable(true);
       action->setStatusTip(tr("Eject record"));
       playerMenu->addSeparator();
       playerToolBar->addAction(action);
       playerToolBar->addSeparator();

       const QIcon record_Icon = QIcon::fromTheme("", QIcon(":/images/resource/record_32px.png"));
       action = playerMenu->addAction(record_Icon, tr("Record"));
       action->setCheckable(true);
       action->setStatusTip(tr("Record"));
       playerMenu->addSeparator();
       playerToolBar->addAction(action);
       playerToolBar->addSeparator();

       const QIcon play_Icon = QIcon::fromTheme("", QIcon(":/images/resource/play_32px.png"));
       action = playerMenu->addAction(play_Icon, tr("Play"));
       action->setCheckable(true);
       action->setStatusTip(tr("Play"));
       playerToolBar->addAction(action);
       playerToolBar->addSeparator();

       const QIcon pause_Icon = QIcon::fromTheme("", QIcon(":/images/resource/pause_button_32px.png"));
       action = playerMenu->addAction(pause_Icon, tr("Pause"));
       action->setCheckable(true);
       action->setStatusTip(tr("Pause"));
       playerToolBar->addAction(action);
       playerToolBar->addSeparator();

       const QIcon rewind_Icon = QIcon::fromTheme("", QIcon(":/images/resource/rewind_32px.png"));
       action = playerMenu->addAction(rewind_Icon, tr("Rewind"));
       action->setCheckable(true);
       action->setStatusTip(tr("Rewind"));
       playerToolBar->addAction(action);

       QSlider *slider = new QSlider();
       slider->setFixedWidth(360);
       slider->setOrientation(Qt::Horizontal);
       playerToolBar->addWidget(slider);

       const QIcon fast_forword_Icon = QIcon::fromTheme("", QIcon(":/images/resource/fast_forward_round_32px.png"));
       action = playerMenu->addAction(fast_forword_Icon, tr("Fast Forword"));
       action->setCheckable(true);
       action->setStatusTip(tr("Fast Forword"));
       playerToolBar->addAction(action);

   //! [Window View]
       QMenu *windowMenu = menuBar()->addMenu(tr("Window(W)"));

       const QIcon casacde_Icon = QIcon::fromTheme("", QIcon(":/images/resource/cascade_32px.png"));
       action = windowMenu->addAction(casacde_Icon, tr("Cascade"));
       action->setCheckable(true);
       action->setStatusTip(tr("Arranage windows so they overlap"));

       const QIcon horiuzontally_Icon = QIcon::fromTheme("", QIcon(":/images/resource/border_horizontal_32px.png"));
       action = windowMenu->addAction(horiuzontally_Icon, tr("Tile Horizontally"));
       action->setCheckable(true);
       action->setStatusTip(tr("Arranage windows so non-overlapping tiles"));

       const QIcon vertically_Icon = QIcon::fromTheme("", QIcon(":/images/resource/border_vertical_32px.png"));
       action = windowMenu->addAction(vertically_Icon, tr("Tile Vertically"));
       action->setCheckable(true);
       action->setStatusTip(tr("Arranage windows so non-overlapping tiles"));

       action = windowMenu->addSeparator();

       action = windowMenu->addAction(tr("Close All"));
       action->setStatusTip(tr("Close All Open Views"));

   //! [Help]
       QMenu *helpMenu = menuBar()->addMenu(tr("Help(H)"));

       const QIcon about_Icon = QIcon::fromTheme("", QIcon(":/images/resource/help_32px.png"));
       action = helpMenu->addAction(about_Icon, tr("About CHManager"));
       action->setCheckable(true);
       action->setStatusTip(tr("Display CHManager information, version and Copyright"));

       action = helpMenu->addSeparator();

       const QIcon contact_Icon = QIcon::fromTheme("", QIcon(":/images/resource/user_groups_32px.png"));
       action = helpMenu->addAction(contact_Icon, tr("Contact HiPNUC"));
       action->setCheckable(true);
       action->setStatusTip(tr("Contact HiPNUC"));

       action = helpMenu->addSeparator();

       const QIcon upgrade_Icon = QIcon::fromTheme("", QIcon(":/images/resource/available_updates_32px.png"));
       action = helpMenu->addAction(upgrade_Icon, tr("Checking Available Upgrade"));
       action->setCheckable(true);
       action->setStatusTip(tr("Checking Available Upgrade"));

   //! [Line Action]
       QToolBar *lineBar = addToolBar(tr("Lines"));

       const QIcon screenshot_Icon = QIcon::fromTheme("", QIcon(":/images/resource/screenshot_32px.png"));
       action = lineBar->addAction(screenshot_Icon, tr("Screenshot"));
       action->setCheckable(true);
       action->setStatusTip(tr("Screenshot current View Chart"));

       const QIcon erase_Icon = QIcon::fromTheme("", QIcon(":/images/resource/erase_32px.png"));
       action = lineBar->addAction(erase_Icon, tr("Clear Chart"));
       action->setCheckable(true);
       action->setStatusTip(tr("Clear current workspace View Chart"));

       const QIcon setting_Icon = QIcon::fromTheme("", QIcon(":/images/resource/settings_32px.png"));
       action = lineBar->addAction(setting_Icon, tr("Chart Setting"));
       action->setCheckable(true);
       action->setStatusTip(tr("Chart Setting"));

       const QIcon pen_Icon = QIcon::fromTheme("", QIcon(":/images/resource/line_width_32px.png"));
       action = lineBar->addAction(pen_Icon, tr("Pen"));
       action->setCheckable(true);
       action->setStatusTip(tr("Change the Specify View Chart Lines option"));
}

void Dashboard::setupStatusBar()
{
    curr_device = new QLabel(tr("Disconnected"));
    curr_datatime = new QLabel(tr("N/A"));

    curr_device->setAutoFillBackground(true);
    curr_device->setAlignment(Qt::AlignHCenter|Qt::AlignCenter);

    curr_datatime->setAutoFillBackground(true);
    curr_datatime->setFixedWidth(120);
    curr_datatime->setAlignment(Qt::AlignHCenter);
    curr_datatime->setToolTip("Time in current country");

    statusBar()->addPermanentWidget(curr_device);
    statusBar()->addPermanentWidget(curr_datatime);
    statusBar()->showMessage(tr("Ready"));

    QTimer *timer = new QTimer(this);
    timer->setInterval(100);
    timer->start();
    connect(timer,SIGNAL(timeout()),this,SLOT(getCurrentTime()));
}

void Dashboard::setupDockWindows()
{
    realdata_dock = new QDockWidget(tr("Real Time Packects"), this);
    realdata_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, realdata_dock);

    heading_dock = new QDockWidget(tr("Heading"), this);
    heading_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, heading_dock);
    heading_dock->hide();

    attitude_dock = new QDockWidget(tr("2D Attitude"), this);
    attitude_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, attitude_dock);
    attitude_dock->hide();

    threed_dock = new QDockWidget(tr("3D Attitude"), this);
    threed_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::TopDockWidgetArea, threed_dock);
    threed_dock->hide();

    atomspheric_dock = new QDockWidget(tr("Atomspheric"), this);
    atomspheric_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::TopDockWidgetArea, atomspheric_dock);
    atomspheric_dock->hide();

    framrate_dock = new QDockWidget(tr("Frame Rate"), this);
    framrate_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::TopDockWidgetArea, framrate_dock);
    framrate_dock->hide();
}

void Dashboard::docktoggleView(bool b)
{
    QAction *action =  qobject_cast <QAction *>(sender());
    QDockWidget *q = nullptr;

    if (action->objectName() == "act_realtime") {
        q = this->realdata_dock;
    }
    else if (action->objectName() == "act_heading")
        q = this->heading_dock;
    else if (action->objectName() == "act_attitude")
        q = this->attitude_dock;
    else if (action->objectName() == "act_threed")
        q = this->threed_dock;
    else if (action->objectName() == "act_atomspheric")
        q = this->atomspheric_dock;
    else if (action->objectName() == "act_framrate")
        q = this->framrate_dock;

    if (b == q->isHidden()) {
        if (b)
            q->show();
        else
            q->close();
    }
}

void Dashboard::getCurrentTime()
{
    QDateTime now = QDateTime::currentDateTime();
    QString time = now.toString("yyyy" + tr("/") + "MM" + tr("/") + "dd" + "," + "hh:mm:ss");
    curr_datatime->setText(time);
//    curr_datatime->setFont(QFont("楷体", 10, QFont::Bold));
    curr_datatime->setAlignment(Qt::AlignHCenter|Qt::AlignCenter);
}

void Dashboard::setTranslator(QTranslator* translator)
{
    this->translator = translator;
}

void Dashboard::switchLanguage(QAction* action)
{
    translator->load(QString(":/languages/CHManager_%1").arg(action->text()));
}

void Dashboard::setCurrentBaudrate(QAction* action)
{
    this->m_baudrate = action->text().toInt();
}

int Dashboard::getCurrentBaudrate()
{
    return this->m_baudrate;

}

void Dashboard::openSerialPort(QAction *action)
{
    if (!ch_dev.openSerialPort(action->text(), getCurrentBaudrate())) {
        curr_device->setText(tr("%1 - Connected").arg(action->text()));
        action->setChecked(true);
    }else {
        curr_device->setText(tr("Cannot connect %1").arg(action->text()));
        action->setChecked(false);
    }

    imu_data_decode_init();
}

void Dashboard::closeSerialPort()
{
    ch_dev.closeSerialPort();
    curr_device->setText(tr("Disconnected"));
}

void Dashboard::readData()
{
    QByteArray array;
    long long len = 0;
    ch_dev.readData(&array, &len);

    if (len > 0) {
        for (int i = 0 ; i < len; i++) {
            uint8_t ch = array[i];
            kptl_decode(ch);
        }
    }
}

void Dashboard::showConsole()
{
    //console->show();
    grid->addWidget(console);
    this->console->transaction();
}

void Dashboard::showCustomchart()
{

}
