#include "connector.h"
#include <QTimer>
#include <QDebug>

Connector::Connector(QObject *parent) : QObject(parent),
    m_socket(new QWebSocket()),
    m_url("ws://192.168.101.101:8001/ws"),
    m_reconnectTimer(new QTimer(this))
{
    connect(m_socket, &QWebSocket::connected, this, &Connector::onSocketConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &Connector::onSocketDisconnected);
    connect(m_socket, &QWebSocket::textMessageReceived, this, &Connector::onSocketMessageReceived);
    connect(m_reconnectTimer, &QTimer::timeout, this, [=]() {
        m_socket->abort();
        m_socket->open(m_url);
    });
}

Connector::~Connector()
{
    delete m_socket;
}

void Connector::setConnectCallback(std::function<void (bool)> callback)
{
    m_callback = callback;
}

void Connector::setJsonParserCallback(std::function<void (const QString &)> callback)
{
    m_jsonParser = callback;
}

void Connector::setSendCallback(std::function<void ()> callback)
{
    m_sendCallback = callback;
}

void Connector::open()
{
    m_socket->open(m_url);
}

void Connector::onSocketDisconnected()
{
    qDebug() << "disconnected ...";
    m_callback(false);
}

void Connector::onSocketConnected()
{
    qDebug() << "connected ...";
    m_callback(true);
    m_sendCallback();
}

void Connector::onSocketMessageReceived(const QString &message)
{
    m_jsonParser(message);
}

void Connector::sendSocketMessage(const QByteArray &data)
{
    /*qDebug() << "size : " << */m_socket->sendBinaryMessage(data);
}
