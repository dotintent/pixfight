//
//  PFSocketClient.hpp
//  PFServer
//
//  Created by Marcin Małysz on 17/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <memory>
#include <vector>

#include "ActiveSocket.h"
#include "PFServerCommandType.hpp"
#include "PFServerUtils.hpp"

class PFSocketClient final {

    static const uint32_t headerSize;

public:

    PFSocketClient(CActiveSocket *socket);
    ~PFSocketClient() noexcept = default;

    PFSocketClient(const PFSocketClient& other) = delete;
    PFSocketClient(PFSocketClient&& other) noexcept = delete;
    PFSocketClient& operator= (const PFSocketClient& other) = delete;
    PFSocketClient& operator= (PFSocketClient&& other) noexcept = delete;

    bool isReceiving() const;
    bool isSending() const;
    bool isValid() const;

    void close();

    inline const char *getLastError() {
        return _socket ? _socket->DescribeError() : "Socket not exist!";
    }

    void update();
    void dispose();

    PFSocketCommandType getCurrentCommand() const { return _command; }
    std::vector<uint8_t> getPacketData() const;
    const PFPacket& getCurrentPacket() const { return _localHeader; }

    void sendPacket(const std::unique_ptr<PFPacket> &packet);

    void ping() { _lastPing = time(0); }
    time_t getCurrentTime() const { return _lastPing; }

    void setReady(bool ready) { _ready = ready; }
    bool isReady() { return _ready; }

private:

    std::unique_ptr<CActiveSocket> _socket;
    PFSocketCommandType _command;

    uint64_t _recivedBytes;
    uint64_t _sendBytes;
    uint64_t _sendSize;

    std::vector<uint8_t> _recivedData;
    std::vector<uint8_t> _data;

    PFPacket _localHeader;

    time_t _lastPing;
    bool _ready;
};
