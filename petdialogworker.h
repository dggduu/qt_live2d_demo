#ifndef PETDIALOGWORKER_H
#define PETDIALOGWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class PetDialogWorker : public QObject {
    Q_OBJECT
public:
    explicit PetDialogWorker(const QString &message, QObject *parent = nullptr);

    signals:
        void finished(const QString &cleanContent);
    void errorOccurred(const QString &errorMessage);

    private slots:
        void replyFinished(QNetworkReply *reply);

private:
    QString message;
    QNetworkAccessManager *manager;
};

#endif // PETDIALOGWORKER_H