#include "message_manager.h"
#include "connector.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>
#include <QTimer>

#ifndef RESULT_FUNC
#define RESULT_FUNC(VAL) ((VAL==1)?"OK":"Error")
#endif

MessageManager *MessageManager::getInstance()
{
    static MessageManager instance;
    return &instance;
}

void MessageManager::sendFirstRequest()
{
    QJsonObject obj;
    obj.insert("Msg", "Calibration");
    QJsonDocument json_document;
    json_document.setObject(obj);
    QByteArray byte_array = json_document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);
    m_connector->sendSocketMessage(byte_array);
}

void MessageManager::open()
{
    m_connector->open();
}

void MessageManager::setConnectCallback(std::function<void (bool)> callback)
{
    m_connector->setConnectCallback(callback);
}

void MessageManager::setBadAckFromServerCallback(std::function<void (const QString &)> callback)
{
    this->m_ack = callback;
}

void MessageManager::setRecordCallback(std::function<void (const std::string &)> start, std::function<void ()> stop)
{
    this->m_start = start;
    this->m_stop = stop;
}

void MessageManager::parseJsonData(const QString &message)
{
//    qDebug() << "message : " << message;
    auto data = message.simplified().trimmed();
    QJsonParseError jsonerror;
    auto doc = QJsonDocument::fromJson(data.toLatin1(), &jsonerror);
    auto json = doc.object();
    if(jsonerror.error == QJsonParseError::NoError) {
        if(json.contains("functionSw") && json.contains("cfgValue")) {
            m_json = json;
            QFile file("./config.json");
            if(file.open(QIODevice::WriteOnly)) {
                file.write(message.toLatin1());
            }
            file.close();

            if(m_json["functionSw"].toObject()["rawDataSw"].toInt() == 0) {
                QMessageBox::information(nullptr,"info","Raw data Switch is OFF");
            }
            else if(m_json["functionSw"].toObject()["rawDataSw"].toInt() == 1) {
                qDebug() << "Raw data Switch is ON";
            }
        }
        else if(json.contains("Msg")) {
            auto ack = QString("%1 -> %2").arg(json["Msg"].toString()).arg(RESULT_FUNC(json["res"].toInt()));
            if(!json["res"].toInt()) {
                this->m_ack(ack);
            }
            qDebug() << ack;
        }
    }
    else {
        qDebug() << "json parse error : " << message;
    }
}

bool MessageManager::checkJsonData(const QString &message)
{
    auto data = message.simplified().trimmed();
    QJsonParseError jsonerror;
    auto doc = QJsonDocument::fromJson(data.toLatin1(), &jsonerror);
    if(jsonerror.error == QJsonParseError::NoError) {
        m_json = doc.object();

        // 合法性检查
        if(!m_json.contains("functionSw")) return false;
        if(!m_json.contains("cfgValue")) return false;
        if(!m_json["functionSw"].toObject().contains("dynamicSw")) return false;
        if(!m_json["functionSw"].toObject().contains("rawDataSw")) return false;

        m_backup = m_json;
        return true;
    }
    return false;
}

void MessageManager::runCommand()
{
    closeDynamicPower();

    m_count = 0;
    auto json = m_json["cfgValue"].toObject();
    for(auto &key : json.keys()) {
        QJsonObject obj;
        obj.insert("Msg", "Calibration");
        QJsonObject node;
        node.insert(key, json[key]);
        obj.insert("cfgValue", node);
        QJsonDocument json_document;
        json_document.setObject(obj);
        QByteArray byte_array = json_document.toJson(QJsonDocument::Compact);
        QString json_str(byte_array);
        m_connector->sendSocketMessage(byte_array);
    }
}

void MessageManager::startToCollect(const QString &name)
{
    m_start(name.toStdString());
    QJsonObject obj;
    obj.insert("Msg", "Calibration");
    QJsonObject node;
    QString key = QString("rawDataSw");
    node.insert(key, 1);
    obj.insert("functionSw", node);
    QJsonDocument json_document;
    json_document.setObject(obj);
    QByteArray byte_array = json_document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);
    m_connector->sendSocketMessage(byte_array);
    m_timer->start();
}

void MessageManager::closeDynamicPower()
{
    QJsonObject obj;
    obj.insert("Msg", "Calibration");
    QJsonObject node;
    QString key = QString("dynamicSw");
    node.insert(key, 0);
    obj.insert("functionSw", node);
    QJsonDocument json_document;
    json_document.setObject(obj);
    QByteArray byte_array = json_document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);
    m_connector->sendSocketMessage(byte_array);
}

void MessageManager::setTimerCount(int timerCount)
{
    this->m_timerCount = timerCount * 1000;
    m_timer->setInterval(this->m_timerCount);
    m_timer->setSingleShot(true);
}

MessageManager::MessageManager(QObject *parent) : QObject(parent), m_connector(new Connector(this)), m_timer(new QTimer(this)), m_count(0)
{
    m_connector->setJsonParserCallback(std::bind(&MessageManager::parseJsonData, this, std::placeholders::_1));
    m_connector->setSendCallback(std::bind(&MessageManager::sendFirstRequest, this));
    QObject::connect(m_timer, &QTimer::timeout, this, [&]() {
        this->m_stop();
        QMessageBox::information(nullptr,"info","Pcap has been saved to local path");
    });
}

MessageManager::~MessageManager()
{

}
