//
// Created by aobara on 25-2-16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_settingElaWin.h" resolved

#include "settingelawin.h"
#include <QScreen>

settingElaWin::settingElaWin(QWidget *parent) :
    QMainWindow(parent){
    QScreen * screen = QGuiApplication::primaryScreen();
    this->setFixedSize(400, 600);
    this->move(screen->availableGeometry().width()/2, screen->availableGeometry().height()/2);
}

settingElaWin::~settingElaWin() {
}
