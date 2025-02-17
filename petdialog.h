#ifndef PETDIALOG_H
#define PETDIALOG_H

#include <QWidget>
#include <QThread>
#include "ElaPlainTextEdit.h"
#include "PetDialogWorker.h"
class petDialog : public QWidget {
    Q_OBJECT

public:
    explicit petDialog(QString message, QWidget *parent = nullptr);
    ~petDialog() override;
    signals:
        void emitsHandledOver();
    private slots:
        void handleFinished(const QString &cleanContent);
        void handleError(const QString &errorMessage);


private:
    bool isAIGenerating=false;
    QThread workerThread;
    PetDialogWorker *worker;
    ElaPlainTextEdit *petDialogBox;
};

#endif