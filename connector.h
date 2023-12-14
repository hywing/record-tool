#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QObject>
#include <QWebSocket>

class QTimer;

class Connector : public QObject
{
    Q_OBJECT
public:
    explicit Connector(QObject *parent = nullptr);

    ~Connector();

    void setConnectCallback(std::function<void (bool)> callback);

    void setJsonParserCallback(std::function<void (const QString &message)> callback);

     void setSendCallback(std::function<void ()> callback);

public slots:
    void open();
    void onSocketDisconnected();
    void onSocketConnected();
    void onSocketMessageReceived(const QString &message);
    void sendSocketMessage(const QByteArray &data);

private:
    QWebSocket *m_socket;
    QUrl m_url;
    QTimer *m_reconnectTimer;
    std::function<void (bool)> m_callback;
    std::function<void (const QString &)> m_jsonParser;
    std::function<void ()> m_sendCallback;
};

#endif // CONNECTOR_H
