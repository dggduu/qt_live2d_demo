#include "PetDialogWorker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QRegularExpression>
#include <QDebug>
#include <QThread>
PetDialogWorker::PetDialogWorker(const QString &message, QObject *parent)
    : QObject(parent), message(message) {
    manager = new QNetworkAccessManager(this); // 在工作线程中创建 QNetworkAccessManager
    connect(manager, &QNetworkAccessManager::finished, this, &PetDialogWorker::replyFinished);

    qDebug() << "Starting work in thread:" << QThread::currentThreadId();
    QUrl url("http://127.0.0.1:45327/v1/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject messageObject;
    messageObject["model"] = "repository@q6_k";
    messageObject["temperature"] = 1;
    messageObject["max_tokens"] = -1;
    messageObject["stream"] = false;

    QJsonArray messages;
    messages.append(QJsonObject{{"role", "system"}, {"content", "Always answer in rhymes."}});
    messages.append(QJsonObject{{"role", "user"}, {"content", message}});

    messageObject["messages"] = messages;

    manager->post(request, QJsonDocument(messageObject).toJson());
}


void PetDialogWorker::replyFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);

        QString rawContent = doc.object()["choices"].toArray().at(0).toObject()["message"].toObject()["content"].toString();
        QRegularExpression thinkBlockRegex("<think>.*?</think>\\n\\n", QRegularExpression::DotMatchesEverythingOption);
        QString cleanContent = rawContent;
        cleanContent.replace(thinkBlockRegex, "");

        emit finished(cleanContent);
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}