#include "streamer.h"
#include "packet_receiver.h"
#include "packet_file_writer.h"
#include "synchronized_queue.h"

#ifndef PCL_DATA_PORT
#define PCL_DATA_PORT 51180
#endif

#ifndef DEV_DATA_PORT
#define DEV_DATA_PORT 8866
#endif

Streamer::Streamer(QObject *parent) : QObject(parent)
{
    m_receiver = std::make_unique<PacketReceiver>();
    m_receiver->bind(DEV_DATA_PORT);
    m_receiver->setReceiverCallback(std::bind(&Streamer::enqueueDev, this, std::placeholders::_1));
}

Streamer::~Streamer()
{

}

void Streamer::start()
{
    m_receiver->start();
}

void Streamer::startRecording(const std::string &dev)
{
    this->stopRecording();
    this->m_writer = std::make_shared<PacketFileWriter>();
    this->m_writer->start(dev);
}

void Streamer::stopRecording()
{
    if (this->m_writer)
    {
        this->m_writer->stop();
        this->m_writer.reset();
    }
}

bool Streamer::isRecording()
{
    return (this->m_writer && this->m_writer) ? true : false;
}

void Streamer::enqueuePcl(NetworkPacket *packet)
{
    if(this->m_writer)
        this->m_writer->enqueue(packet);
}

void Streamer::enqueueDev(NetworkPacket *packet)
{
    if(this->m_writer)
        this->m_writer->enqueue(packet);
}
