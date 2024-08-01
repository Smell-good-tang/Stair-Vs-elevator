#ifndef SINGLEINSTANCEAPP_H
#define SINGLEINSTANCEAPP_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalServer>
#include <QLocalSocket>

#ifdef _WIN32
void activateExistingWindow();
#endif

class SingleInstanceApp : public QObject
{
    Q_OBJECT
public:
    explicit SingleInstanceApp(QObject *parent = nullptr);
    ~SingleInstanceApp();

signals:
    void showExistingWindow();

private slots:
    void onNewConnection();

public:
    QLocalSocket *socket      = nullptr;
    QLocalServer *localServer = nullptr;
};

#endif  // SINGLEINSTANCEAPP_H
