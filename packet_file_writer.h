// Copyright 2013 Velodyne Acoustics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PACKETWRITER_H
#define PACKETWRITER_H

#include <string>
#include <queue>
#include <boost/asio.hpp>
#include "synchronized_queue.h"
#include "network_packet.h"
#include "pcap_writer.h"

class PacketFileWriter
{
public:
    void ThreadLoop();

    void start(const std::string& filename);

    void stop();

    void enqueue(NetworkPacket* packet);

    bool isOpen() { return this->PacketWriter.IsOpen(); }

    void close() { this->PacketWriter.Close(); }

private:
    PcapWriter PacketWriter;
    std::unique_ptr<std::thread> Thread;
    std::shared_ptr<SynchronizedQueue<NetworkPacket *>> Packets;
    int m_count = 0;
};


#endif // PACKETWRITER_H
