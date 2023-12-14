#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QObject>
#include <functional>
#include <QJsonObject>

class Connector;
class QTimer;

class MessageManager : public QObject
{
    Q_OBJECT
public:
    static MessageManager *getInstance();

    void sendFirstRequest();

    void open();

    void setConnectCallback(std::function<void (bool)> callback);

    void setBadAckFromServerCallback(std::function<void (const QString &)> callback);

    void setRecordCallback(std::function<void (const std::string &)> start, std::function<void ()> stop);

    void parseJsonData(const QString &message);

    bool checkJsonData(const QString &data);

    void runCommand();

    void startToCollect(const QString &name);

    void closeDynamicPower();

    void setTimerCount(int timerCount);

protected:
    explicit MessageManager(QObject *parent = nullptr);

    virtual ~MessageManager();

private:
    Connector *m_connector;
    QJsonObject m_json;
    QJsonObject m_backup;
    std::function<void (const std::string &)> m_start;
    std::function<void (const QString &)> m_ack;
    std::function<void ()> m_stop;
    QTimer *m_timer;
    uint m_count;
    int m_timerCount;
};

#endif // MESSAGEMANAGER_H
