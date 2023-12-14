#ifndef PACKETRECEIVER_H
#define PACKETRECEIVER_H

#include <QObject>
#include <boost/asio.hpp>
#include "network_packet.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 34000
#endif

class PacketReceiver : public QObject
{
    Q_OBJECT
public:
    explicit PacketReceiver(QObject *parent = nullptr);

    virtual ~PacketReceiver();

    void start();

    void stop();

    void setReceiverCallback(std::function<void(NetworkPacket *)> callback);

    void bind(int port);

protected:
    void socketCallback(const boost::system::error_code& error, std::size_t numberOfBytes);

    void waitForNextPacket();

private:
    int Port {-1};
    uint64_t FakeManufacturerMACAddress {0};
    unsigned char RXBuffer[BUFFER_SIZE] = {0};
    std::string LocalListeningAddress {"::"};
    boost::asio::io_service IOService;
    boost::asio::ip::udp::endpoint SenderEndpoint;
    boost::asio::ip::udp::socket Socket;
    std::unique_ptr<std::thread> Thread;
    std::function<void(NetworkPacket *)> ReceiverCallback;
};

#endif // PACKETRECEIVER_H
