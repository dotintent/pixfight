//
//  PFMultiplayerClient.hpp
//  PFClient
//
//  Created by Marcin Małysz on 01/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <iostream>
#include <thread>

#include "PFServerCommandType.hpp"
#include "PFSocketClient.hpp"

class PFMultiplayerClient final {

    typedef std::function<bool(const PFSocketCommandType &command, const std::vector<uint8_t> data)> PFCallback;

public:

    PFMultiplayerClient(const std::string &serverAddress, bool async);
    
    ~PFMultiplayerClient() noexcept;

    PFMultiplayerClient(const PFMultiplayerClient& other) = delete;
    PFMultiplayerClient(PFMultiplayerClient&& other) noexcept = delete;
    PFMultiplayerClient& operator= (const PFMultiplayerClient& other) = delete;
    PFMultiplayerClient& operator= (PFMultiplayerClient&& other) noexcept = delete;

    void handleCommand(PFCallback callback);

    void sendFireCommand(uint32_t unitID,
                         uint32_t attackedID,
                         uint32_t sizeA,
                         uint32_t sizeB);
    void moveUnitCommand(uint32_t uintID, float x, float y);
    void buildUnitCommand(uint32_t baseID, uint16_t unitType);

    void tick(); //can be use inside game loop to avoid another thread

private:

    //it will run on dedicated thread
    void loop();
    void update();

private:

    std::string _serverAddress;
    uint32_t _port;
    std::unique_ptr<PFSocketClient> _socket;

    PFRoomInfo _roomDetails;

    std::thread _loopThread;
    std::atomic_bool _threadTerminate;
};
