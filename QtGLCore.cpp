#include <LAppDelegate.hpp>
#include <LAppView.hpp>
#include <qtimer.h>
#include <qmouseevent>
#include <QtGLCore.h>
#include <qdebug.h>
#include <elaWidget/ElaIcon.h>
#include <settingelawin.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <qscreen.h>

QtGLCore::QtGLCore(QWidget* parent)
	: QOpenGLWidget(parent)
	,manager(new QNetworkAccessManager(this))
	,trayIconMenu()
	,inputLine()
	,aiDialog()
{
	QSurfaceFormat format;
	format.setAlphaBufferSize(8);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	setFormat(format);

	this->setWindowFlag(Qt::FramelessWindowHint);
	this->setWindowFlag(Qt::WindowStaysOnTopHint);
	this->setWindowFlag(Qt::Tool);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setFixedSize(500,500);

	moveToBottomRight();
	initMenu();
	
	timer = new QTimer(this);
	connect(timer,&QTimer::timeout,this, [=]() {
		update();
		});
	timer->start(1000/60);

	randMessageManager();
}

QtGLCore::~QtGLCore()
{
	delete timer;
}

void QtGLCore::moveToBottomRight() {
	QScreen* screen = QApplication::primaryScreen();

	if (!screen) {
		qDebug() << "Failed to get the primary screen.";
		return;
	}

	QRect screenGeometry = screen->availableGeometry();

	QSize windowSize = this->size();
	int x = screenGeometry.right() - windowSize.width();
	int y = screenGeometry.bottom() - windowSize.height();

	this->move(x, y);
}

void QtGLCore::initializeGL()
{
	LAppDelegate::GetInstance()->Initialize(this);

}

void QtGLCore::resizeGL(int w, int h)
{
	LAppDelegate::GetInstance()->resize(w, h);
}

void QtGLCore::paintGL()
{
	LAppDelegate::GetInstance()->update();
}

void QtGLCore::mousePressEvent(QMouseEvent* event)
{
	LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(static_cast<float>(event->position().x()), static_cast<float>(event->position().y()));
	if (event->button() == Qt::LeftButton) {
		this->isLeftButton = true;
		this->currentPos = event->pos();
	}
}


void QtGLCore::initMenu() {
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setToolTip("live2dViewer");
	connect(trayIcon, &QSystemTrayIcon::activated, this, &QtGLCore::iconActivated);

	trayIconMenu = new ElaMenu(this);
	trayIconMenu->setFixedWidth(150);
	QAction* switchAction = trayIconMenu->addElaIconAction(ElaIconType::ComputerMouse,"鼠标跟踪");
	switchAction->setCheckable(true);
	connect(switchAction, &QAction::triggered, [=]() {
		if (switchAction->isChecked()) {
			this->setMouseTracking(true);
		}else {
			this->setMouseTracking(false);
		}
	});
	QAction * HitokotoEnableSwitch = trayIconMenu->addElaIconAction(ElaIconType::Message,"启用一言");
	HitokotoEnableSwitch->setCheckable(true);
	connect(HitokotoEnableSwitch, &QAction::triggered, [=]() {
		if (HitokotoEnableSwitch->isChecked()) {
			isUseHitokoto = true;
		} else {
			isUseHitokoto = false;
		}
	});
	QAction* settingAction = trayIconMenu->addElaIconAction(ElaIconType::Gear,"设置");
	connect(settingAction, &QAction::triggered, [=]() {
		qDebug() << "Setting open";
		 settingElaWin *setting =new settingElaWin(this);
		setting->show();
	});
	QAction* quitAction = trayIconMenu->addElaIconAction(ElaIconType::Xmark,"退出");

	connect(quitAction, &QAction::triggered, this, &QtGLCore::quitApplication);
	connect(settingAction,&QAction::triggered,this,&QtGLCore::openSettingWindow);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setIcon(QIcon(":image/1.png"));
	trayIcon->show();

}

void QtGLCore::openSettingWindow() {
	
}
void QtGLCore::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
		case QSystemTrayIcon::Trigger:
			if (this->isVisible()) {
				fadeOutAnimation();
			} else {
				this->setVisible(true);
				showEvent(nullptr);
			}
		break;
		default:
			;
	}
}
void QtGLCore::showEvent(QShowEvent * event) {					//缓入动画（1s）
	QPropertyAnimation *animetion = new QPropertyAnimation(this, "windowOpacity");
	animetion->setDuration(1000);
	animetion->setStartValue(0.0);
	animetion->setEndValue(1.0);
	animetion->start(QPropertyAnimation::DeleteWhenStopped);
}
void QtGLCore::fadeOutAnimation() {
	QPropertyAnimation *fadeAnimation = new QPropertyAnimation(this, "windowOpacity");
	fadeAnimation->setDuration(1000);
	fadeAnimation->setStartValue(1.0);  // 从完全不透明开始
	fadeAnimation->setEndValue(0.0);    // 到完全透明结束
	fadeAnimation->start(QPropertyAnimation::DeleteWhenStopped);
	connect(fadeAnimation, &QPropertyAnimation::finished, [=]() {
		this->setVisible(false);
	});
}
void QtGLCore::quitApplication() {								//缓出动画（500ms）
	animation = new QPropertyAnimation(this, "windowOpacity");
	animation->setDuration(500);
	animation->setStartValue(0.0);
	animation->setEndValue(1.0);
	connect(animation, &QPropertyAnimation::finished, [=]() {
		animation->deleteLater();
		QApplication::quit();
		});
	animation->start(QPropertyAnimation::DeleteWhenStopped);
}


void QtGLCore::mouseReleaseEvent(QMouseEvent* event)
{
	LAppDelegate::GetInstance()->GetView()->OnTouchesEnded(static_cast<float>(event->position().x()), static_cast<float>(event->position().y()));
	if (event->button() == Qt::LeftButton) {
		this->isLeftButton = false;
	}
}

void QtGLCore::mouseMoveEvent(QMouseEvent* event)
{
	LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(event->position().x(), event->position().y());
	if (this->isLeftButton) {
		this->move(event->pos() - this->currentPos + this->pos());
	}
}

void QtGLCore::keyReleaseEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Alt) {
		if (isOpeningLineEdit) {
			if (inputLine != nullptr) {
				inputLine->hide();
				delete inputLine;
				aiDialog->hide();
				delete aiDialog;
				inputLine = nullptr;
			}
			isOpeningLineEdit = false;

		} else {
			inputLine = new ElaLineEdit(this);
			inputLine->setFixedWidth(200);
			inputLine->setMinimumHeight(30);
			inputLine->setFont(QFont("Maple Hand",10));
			inputLine->move(0,400);
			inputLine->show();
			inputLine->setFocus();
			isOpeningLineEdit = true;

		}
	}else if (event->key()==Qt::Key_Control&&isOpeningLineEdit) {
		qDebug() << "开始子进程，发送请求";
		QString userInput = inputLine->text();
		aiDialog = new petDialog(userInput, this);
		connect(aiDialog, &petDialog::emitsHandledOver, [=]() {
			aiDialog->show();
		});
	}
}
void QtGLCore::fetchHitokoto() {
    QUrl url("http://127.0.0.1:8000");
    QNetworkRequest request(url);
    manager->get(request);
}

void QtGLCore::handleReply(QNetworkReply *reply) {
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray response_data = reply->readAll();
		QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
		QJsonObject json_obj = json_doc.object();

		QString hitokoto = json_obj["hitokoto"].toString();
		showMessageBar(hitokoto);
	} else {
		showMessageBar("未找到一言服务");
		qDebug() << "Network error:" << reply->errorString();
	}

	reply->deleteLater();
}
void QtGLCore::showMessageBar(QString message) {
	ElaMessageBar::information(ElaMessageBarType::TopLeft, "", message, 10000, this);
}
void QtGLCore::randMessageManager() {
	connect(manager, &QNetworkAccessManager::finished,
		this, &QtGLCore::handleReply);
	QTimer *messageTimer = new QTimer(this);
	showMessageBar("启动成功");
	connect(messageTimer, &QTimer::timeout, [=]() {
		if (!isOpeningLineEdit) {
					int rand = QRandomGenerator::global()->generate();
			switch (rand %10) {
				case 0:
					showMessageBar("orz");
					break;
				case 1:
					showMessageBar("你好");
					break;
				case 2:
					showMessageBar("记得玩久了要休息一下欧:)");
					break;
				default:
					if (isUseHitokoto) {
						fetchHitokoto();
					}else {
						showMessageBar("可以在设置中启用一言！qwq");
					}
					break;
			}
		}
		}
);
	messageTimer->start(50000);
}