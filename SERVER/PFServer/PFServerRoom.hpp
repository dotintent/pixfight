//
//  PFServerRoom.hpp
//  PFServer
//
//  Created by Marcin Małysz on 18/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "PassiveSocket.h"
#include "ActiveSocket.h"

#include "PFServerCommandType.hpp"
#include "PFSocketClient.hpp"

typedef PF_ENUM(uint16_t, PFRoomStatus) {

    PFRoomStatusAwaiting,
    PFRoomStatusTimeout,
    PFRoomStatusPlaying,
    PFRoomStatusFinished
};

class PFServerRoom final {

public:

    PFServerRoom(uint32_t port, bool privateRoom);
    ~PFServerRoom() noexcept;

    PFServerRoom(const PFServerRoom& other) = delete;
    PFServerRoom(PFServerRoom&& other) noexcept = delete;
    PFServerRoom& operator= (const PFServerRoom& other) = delete;
    PFServerRoom& operator= (PFServerRoom&& other) noexcept = delete;

    void update();
    uint32_t getPort() { return _port; }

    //connection broken / clients problem
    bool isValid();
    bool isPrivate() const { return _private; }

    //room is empty for to much time and should be removed
    bool isUnused();

    PFRoomStatus getStatus() const { return _status; }
    const PFRoomInfo &getRoomInfo() const { return _roomInfo; }

private:

    bool handlePlayer(std::shared_ptr<PFSocketClient> &player);
    void confirmPacket(std::shared_ptr<PFSocketClient> &player);

private:

    uint32_t _port;
    bool _private;
    PFRoomStatus _status;
    PFRoomInfo _roomInfo;
    time_t _lastUpdate;
    std::unique_ptr<CPassiveSocket> _roomSocket;
    CActiveSocket *_client;
    std::vector<std::shared_ptr<PFSocketClient>> _connectedPlayers;
    uint32_t _currentPlayerTurn;
};
