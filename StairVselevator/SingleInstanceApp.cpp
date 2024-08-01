#include "SingleInstanceApp.h"

SingleInstanceApp::SingleInstanceApp(QObject *parent) : QObject(parent)
{
    QString local_server_name = "-|%We_need_unique_local_server_name_for_Client.%|-";
    socket                    = new QLocalSocket(this);
    socket->connectToServer(local_server_name);
    if (socket->waitForConnected(500)) {
        socket->waitForReadyRead();
        QByteArray    data = socket->readAll();
        QJsonDocument doc  = QJsonDocument::fromJson(data);  // 将接收到的数据转换为 JSON 文档
        QJsonObject   json = doc.object();
        // 接收到请求显示信号
        if (json["Keyword"].toString() == "Request_SHOW") {
            // 应用程序已在运行，发送信号并退出
            QJsonObject json;
            json["Keyword"] = "SHOW";

            // 将 JSON 对象转换为字符串
            QJsonDocument doc(json);
            QString       jsonString = doc.toJson(QJsonDocument::Compact);

            // 发送 JSON 字符串
            socket->write(jsonString.toUtf8());
            socket->flush();
            socket->disconnectFromServer();
            exit(0);
        }
    } else {
        // 创建本地服务器
        localServer = new QLocalServer(this);
        connect(localServer, &QLocalServer::newConnection, this, &SingleInstanceApp::onNewConnection);
        localServer->listen(local_server_name);
    }
}

SingleInstanceApp::~SingleInstanceApp()
{
    delete localServer;
    localServer = nullptr;
    delete socket;
    socket = nullptr;
}

void SingleInstanceApp::onNewConnection()
{
    QLocalSocket *clientConnection = localServer->nextPendingConnection();
    connect(clientConnection, &QLocalSocket::readyRead, [this, clientConnection]() {
        QByteArray    data = clientConnection->readAll();
        QJsonDocument doc  = QJsonDocument::fromJson(data);  // 将接收到的数据转换为 JSON 文档
        QJsonObject   json = doc.object();
        if (json["Keyword"].toString() == "SHOW") {
            emit showExistingWindow();
        }
    });
    // 请求重复打开的实例发送过来显示信号
    QJsonObject json;
    json["Keyword"] = "Request_SHOW";

    // 将 JSON 对象转换为字符串
    QJsonDocument doc(json);
    QString       jsonString = doc.toJson(QJsonDocument::Compact);

    // 发送 JSON 字符串
    clientConnection->write(jsonString.toUtf8());
    clientConnection->flush();
}
