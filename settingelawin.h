//
// Created by aobara on 25-2-16.
//

#ifndef SETTINGELAWIN_H
#define SETTINGELAWIN_H

#include <QMainWindow>
#include <elaWidget/ElaWindow.h>


class settingElaWin : public QMainWindow {
Q_OBJECT

public:
    explicit settingElaWin(QWidget *parent = nullptr);
    ~settingElaWin() override;

private:
};


#endif //SETTINGELAWIN_H
