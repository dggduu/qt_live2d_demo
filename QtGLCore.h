#pragma once

#include <QWidget>
#include <QOpenGLWidget>
#include <qapplication.h>
#include <qaction.h>
#include <QSystemTrayIcon>
#include <qpropertyanimation.h>   //animation
#include <QGraphicsOpacityEffect>
#include <elaWidget/ElaMenu.h>
#include <elaWidget/ElaMessageBar.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>>
#include <QKeyEvent>
#include <elaWidget/ElaLineEdit.h>
#include <petdialog.h>
class QtGLCore : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit QtGLCore(QWidget* parent = nullptr);
    ~QtGLCore() override;

protected:
    void moveToBottomRight();
    void showEvent(QShowEvent *event) override;
    void fadeOutAnimation();
    void initializeGL() override;
    void initMenu();
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void openSettingWindow();
    void showMessageBar(QString message);
    void fetchHitokoto();
    void randMessageManager();

    private slots:
        void quitApplication();
        void handleReply(QNetworkReply *reply);
private:
    bool isLeftButton = false;
    bool isRightButton = false;
    bool isUseHitokoto = false;
    bool isOpeningLineEdit =false;
    QPoint currentPos;
    QAction* switchAction{};
    QAction* settingAction{};
    QSystemTrayIcon* trayIcon{};
    ElaMenu* trayIconMenu;
    QPropertyAnimation* animation{};
    QNetworkAccessManager *manager;
    ElaLineEdit *inputLine ;
    petDialog *aiDialog;
    QTimer* timer;
};