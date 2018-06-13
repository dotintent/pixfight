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

    typedef std::function<void(const PFSocketCommandType &command, const std::vector<uint8_t> data)> PFCallback;

public:

    PFCallback callback;

public:

    PFMultiplayerClient(const std::string &serverAddress);
    
    ~PFMultiplayerClient() noexcept;

    PFMultiplayerClient(const PFMultiplayerClient& other) = delete;
    PFMultiplayerClient(PFMultiplayerClient&& other) noexcept = delete;
    PFMultiplayerClient& operator= (const PFMultiplayerClient& other) = delete;
    PFMultiplayerClient& operator= (PFMultiplayerClient&& other) noexcept = delete;

    bool connect();
    void disconnect();

    bool isValid() const { return _socket ? _socket->isValid() : false; }

    //commands
    bool joinRoom(uint32_t roomid);
    bool leaveRoom();
    void makeRoom(bool isPrivate);
    void setRoomInfo(PFRoomInfo &roomInfo);
    void sendRoomDetails();
    void removeRoom();
    void listRooms();
    void setReady();
    void setLoaded();
    void endTurn();

    void sendFireCommand(uint32_t unitID,
                         uint32_t attackedID,
                         uint32_t sizeA,
                         uint32_t sizeB);

    void moveUnitCommand(uint32_t unitID, float x, float y);
    void buildUnitCommand(uint32_t baseID, uint16_t unitType);

    const PFRoomInfo & getRoomInfo() const { return _roomDetails; }
    uint32_t getCurrentPort() const { return _port; }

private:

    //it will run on dedicated thread
    void loop();
    PFSocketCommandType update();

private:

    std::string _serverAddress;
    uint32_t _port;
    std::unique_ptr<PFSocketClient> _socket;

    PFRoomInfo _roomDetails;

    std::thread _loopThread;
    std::atomic_bool _threadTerminate;

    bool _playing;
};
