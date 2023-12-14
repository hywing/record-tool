#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>

class PacketReceiver;
class PacketFileWriter;
class NetworkPacket;

class Streamer : public QObject
{
    Q_OBJECT

public:
    explicit Streamer(QObject *parent = nullptr);

    virtual ~Streamer();

    void start();

    void startRecording(const std::string &dev);

    void stopRecording();

    bool isRecording();

protected:
    void enqueuePcl(NetworkPacket* packet);

    void enqueueDev(NetworkPacket* packet);

private:
    std::unique_ptr<PacketReceiver> m_receiver;

    std::shared_ptr<PacketFileWriter> m_writer;
};

#endif // STREAMER_H
