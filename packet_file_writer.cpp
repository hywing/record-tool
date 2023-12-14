#include "packet_file_writer.h"
#include <QMessageBox>
#include <QDebug>

//-----------------------------------------------------------------------------
void PacketFileWriter::ThreadLoop()
{
    NetworkPacket* packet = nullptr;
    while (this->Packets->dequeue(packet))
    {
        this->PacketWriter.WritePacket(*packet);
        delete packet;
    }
}

//-----------------------------------------------------------------------------
void PacketFileWriter::start(const std::string &filename)
{
    m_count = 0;
    if (this->Thread)
    {
        return;
    }

    if (this->PacketWriter.GetFileName() != filename)
    {
        this->PacketWriter.Close();
    }

    if (!this->PacketWriter.IsOpen())
    {
        if (!this->PacketWriter.Open(filename))
        {
            qDebug() << "Failed to open packet file: ";
            return;
        }
    }

    this->Packets.reset(new SynchronizedQueue<NetworkPacket*>);
    this->Thread = std::make_unique<std::thread>(
                std::mem_fn(&PacketFileWriter::ThreadLoop),
                this
                );
}

//-----------------------------------------------------------------------------
void PacketFileWriter::stop()
{
    if (this->Thread)
    {
        this->Packets->stopQueue();
        this->Thread->join();
        this->Thread.reset();
        this->Packets.reset();

        if(m_count < 20002) {
            qDebug() << "Packets (" << m_count << ") not enough, please record again!";
        }
        else if(m_count == 20002) {
            qDebug() << "Pcap file is ready.";
        }
    }
}

//-----------------------------------------------------------------------------
void PacketFileWriter::enqueue(NetworkPacket* packet)
{
    // TODO
    // After capturing a stream and stoping the recording, Packets is NULL
    // and this loop continues until a new reader or stream is selected.
    if (this->Packets != NULL)
    {
        this->Packets->enqueue(packet);
        m_count++;
    }
    else
    {
        this->stop();
    }
}
