#include "packet_receiver.h"
#include <QDebug>

PacketReceiver::PacketReceiver(QObject *parent) : QObject(parent), Socket(IOService)
{

}

PacketReceiver::~PacketReceiver()
{
    this->stop();
    this->Socket.close();
}

void PacketReceiver::start()
{
    qDebug() << "start to collect udp data : " << this->Port;
    this->waitForNextPacket();
    this->Thread = std::make_unique<std::thread>([&] {
        this->IOService.run();
    });
}

void PacketReceiver::stop()
{
    qDebug() << "stop collcting udp data";
    this->Socket.close();
    if (this->Thread)
    {
        if (this->Thread->joinable())
        {
            this->Thread->join();
        }
    }
}

void PacketReceiver::setReceiverCallback(std::function<void (NetworkPacket *)> callback)
{
    this->ReceiverCallback = callback;
}

void PacketReceiver::bind(int port)
{
    this->Port = port;

    boost::system::error_code errCode;
    boost::asio::ip::address listen_address = boost::asio::ip::address::from_string(LocalListeningAddress, errCode);
    if(errCode != boost::system::errc::success)
    {
        qDebug() << "Listen address is not valid, listening on all local ip addresses on v6";
        listen_address = boost::asio::ip::address_v6::any();
    }

    if (listen_address.is_v4())
    {
        this->Socket.open(boost::asio::ip::udp::v4());
    }
    else if (listen_address.is_v6())
    {
        this->Socket.open(boost::asio::ip::udp::v6());
        if (listen_address == boost::asio::ip::address_v6::any())
        {
            this->Socket.set_option(boost::asio::ip::v6_only(false));
        }
    }

    this->Socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));

    // default 106496 (linux) to 4194304 (4MB)
    this->Socket.set_option(boost::asio::socket_base::receive_buffer_size(4194304));
    boost::asio::socket_base::receive_buffer_size size_option;
    this->Socket.get_option(size_option);
    qDebug() << "Receive buffer size : " << size_option.value();

    try
    {
        this->Socket.bind(boost::asio::ip::udp::endpoint(listen_address, Port));
    }
    catch(...)
    {
        qDebug() << "error while setting listening address, please correct it or leave empty to ignore";
    }
}

void PacketReceiver::socketCallback(const boost::system::error_code &error, std::size_t numberOfBytes)
{
    if (error)
    {
        return;
    }
    unsigned short ourPort = static_cast<unsigned short>(this->Port);
    unsigned short sourcePort = this->SenderEndpoint.port();
    std::array<unsigned char, 4> sourceIP = {192, 168, 0, 200};
    if (this->SenderEndpoint.address().is_v4())
    {
        for (int i = 0; i < 4; i++)
        {
            sourceIP[i] = this->SenderEndpoint.address().to_v4().to_bytes()[i];
        }
    }

    NetworkPacket* packet = NetworkPacket::BuildEthernetIP4UDP(this->RXBuffer, numberOfBytes, sourceIP, sourcePort, ourPort, this->FakeManufacturerMACAddress);

    this->ReceiverCallback(packet);

    this->waitForNextPacket();
}

void PacketReceiver::waitForNextPacket()
{
    this->Socket.async_receive_from(boost::asio::buffer(this->RXBuffer, BUFFER_SIZE), this->SenderEndpoint, std::bind(&PacketReceiver::socketCallback, this, std::placeholders::_1, std::placeholders::_2));
}
