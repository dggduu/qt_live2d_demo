#include "petdialog.h"
#include <QPushButton>
#include <QDebug>
#include <elaWidget/ElaMessageBar.h>

petDialog::petDialog(QString message, QWidget *parent)
    : QWidget(parent), worker(nullptr), isAIGenerating(false) { // 初始化状态变量
    petDialogBox = new ElaPlainTextEdit(this);
    petDialogBox->setGeometry(0, 100, 200, 60);
    petDialogBox->setFixedWidth(200);
    petDialogBox->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    petDialogBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    petDialogBox->ensureCursorVisible();
    QFont font("Maple Hand",10);
    petDialogBox->setFont(font);
    petDialogBox->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    worker = new PetDialogWorker(message);
    worker->moveToThread(&workerThread);

    connect(worker, &PetDialogWorker::finished, this, &petDialog::handleFinished);
    connect(worker, &PetDialogWorker::errorOccurred, this, &petDialog::handleError);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    isAIGenerating = true;
    workerThread.start();
}

petDialog::~petDialog() {
    workerThread.quit();
    workerThread.wait();
}

void petDialog::handleFinished(const QString &cleanContent) {
    qDebug() << "Finished processing in thread:" << QThread::currentThreadId();
    petDialogBox->setPlainText(cleanContent);
    isAIGenerating = false;
    emit emitsHandledOver();
}

void petDialog::handleError(const QString &errorMessage) {
    qDebug() << "Error occurred in thread:" << QThread::currentThreadId();
    ElaMessageBar::error(ElaMessageBarType::TopLeft,"Error", "无法链接AI服务",500,this);
    isAIGenerating = false;
}