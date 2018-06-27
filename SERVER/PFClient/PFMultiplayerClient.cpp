//
//  PFMultiplayerClient.cpp
//  PFClient
//
//  Created by Marcin Małysz on 01/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include <chrono>
#include <thread>

#include "PFMultiplayerClient.hpp"

using namespace std;

PFMultiplayerClient::PFMultiplayerClient(const std::string &serverAddress)
: callback(nullptr)
, _serverAddress(serverAddress)
, _port(DEFAULT_SERVER_PORT)
, _socket(nullptr)
, _roomDetails({})
, _loopThread({})
, _threadTerminate(false)
, _playing(false)
, _allowConnection(true) {

}

PFMultiplayerClient::~PFMultiplayerClient() noexcept {

    disconnect();
    callback = nullptr;
}

bool PFMultiplayerClient::connect() {

    if (_serverAddress.empty()) {

        return false;
    }

    while (false == _allowConnection.load()) {

        this_thread::yield();
    }

    auto socket = new CActiveSocket();
    socket->Initialize();
    socket->SetNonblocking();

    uint16_t retry = 0;

    while (!socket->Open(_serverAddress.c_str(), _port)) {

        this_thread::sleep_for(chrono::milliseconds(200));

        retry++;

        if (retry > 3) {

            cout << "Error connecting: " << socket->DescribeError() << endl;

            return false;
        }
    }

    _socket = unique_ptr<PFSocketClient>(new PFSocketClient(socket));

    _threadTerminate = false;

    //call loop from another thread
    _loopThread = thread(&PFMultiplayerClient::loop, this);
    _loopThread.detach();

    _playing = _port != DEFAULT_SERVER_PORT;

    _allowConnection = false;

    return true;
}

void PFMultiplayerClient::disconnect() {

    _threadTerminate = true;

    if (!_playing && _socket) {

        auto packet = make_unique<PFPacket>();
        packet->type = PFSocketCommandTypeDisconnect;
        _socket->sendPacket(packet);
    }

    _playing = false;
}

bool PFMultiplayerClient::joinRoom(uint32_t roomid) {

    if (_playing) {
        return false;
    }

    _port = roomid;

    disconnect();

    return connect();
}

bool PFMultiplayerClient::leaveRoom() {

    if (!_playing) {
        return false;
    }

    if (_socket) {

        auto packet = make_unique<PFPacket>();
        packet->type = PFSocketCommandTypeLeaveRoom;

        _socket->sendPacket(packet);
    }

    _port = DEFAULT_SERVER_PORT;

    disconnect();

    return connect();
}

void PFMultiplayerClient::makeRoom(bool isPrivate) {

    if (_playing) {
        return;
    }

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeMakeRoom;
    packet->size = sizeof(isPrivate)/sizeof(uint8_t);
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, &isPrivate, sizeof(isPrivate));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::setRoomInfo(PFRoomInfo &roomInfo) {

    _roomDetails = roomInfo;
}

void PFMultiplayerClient::sendRoomDetails() {

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeGameInfo;
    packet->size = sizeof(_roomDetails) / sizeof(uint8_t);
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, &_roomDetails, sizeof(_roomDetails));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::getRoomDetails() {

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeGetGameInfo;
    _socket->sendPacket(packet);
}

void PFMultiplayerClient::removeRoom() {

    if (_playing) {
        return;
    }

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeRemoveRoom;

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::listRooms() {

    if (_playing) {
        return;
    }

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeRooms;

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::setReady() {

    if (!_playing) {
        return;
    }

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeReady;

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::setLoaded() {

    if (!_playing) {
        return;
    }

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeLoad;

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::endTurn() {

    if (!_playing) {
        return;
    }

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeSendTurn;

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::sendWinnerID(uint32_t winnerID) {

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeEndGame;
    packet->size = sizeof(winnerID) / sizeof(uint8_t);
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, &winnerID, sizeof(winnerID));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::sendFireCommand(uint32_t unitID,
                                          uint32_t attackedID,
                                          uint32_t sizeA,
                                          uint32_t sizeB) {

    if (!_playing) {
        return;
    }

    vector<uint8_t> result(sizeof(uint32_t) * 4);

    memcpy(result.data(), reinterpret_cast<const uint8_t*>(&unitID), sizeof(uint32_t));
    memcpy(result.data() + sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&attackedID), sizeof(uint32_t));
    memcpy(result.data() + sizeof(uint32_t) * 2, reinterpret_cast<const uint8_t*>(&sizeA), sizeof(uint32_t));
    memcpy(result.data() + sizeof(uint32_t) * 3, reinterpret_cast<const uint8_t*>(&sizeB), sizeof(uint32_t));

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeFire;
    packet->size = (uint32_t)result.size();
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, result.data(), result.size() * sizeof(uint8_t));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::moveUnitCommand(uint32_t unitID, float x, float y) {

    if (!_playing) {
        return;
    }

    vector<uint8_t> result(sizeof(uint32_t) + sizeof(float) * 2);

    memcpy(result.data(), reinterpret_cast<const uint8_t*>(&unitID), sizeof(uint32_t));
    memcpy(result.data() + sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&x), sizeof(float));
    memcpy(result.data() + sizeof(uint32_t) + sizeof(float), reinterpret_cast<const uint8_t*>(&y), sizeof(float));

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeMove;
    packet->size = (uint32_t)result.size();
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, result.data(), result.size() * sizeof(uint8_t));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::buildUnitCommand(uint32_t baseID, uint16_t unitType) {

    if (!_playing) {
        return;
    }

    vector<uint8_t> result(sizeof(uint32_t) + sizeof(uint16_t));

    memcpy(result.data(), reinterpret_cast<const uint8_t*>(&baseID), sizeof(uint32_t));
    memcpy(result.data() + sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&unitType), sizeof(uint16_t));

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeBuild;
    packet->size = (uint32_t)result.size();
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, result.data(), result.size() * sizeof(uint8_t));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::captureBasecommand(uint32_t baseID, uint32_t unitID) {

    if (!_playing) {
        return;
    }

    vector<uint8_t> result(sizeof(uint32_t) + sizeof(uint32_t));

    memcpy(result.data(), reinterpret_cast<const uint8_t*>(&baseID), sizeof(uint32_t));
    memcpy(result.data() + sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&unitID), sizeof(uint32_t));

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeCapture;
    packet->size = (uint32_t)result.size();
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, result.data(), result.size() * sizeof(uint8_t));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::repairUnitcommand(uint32_t baseID, uint32_t unitID) {

    if (!_playing) {
        return;
    }

    vector<uint8_t> result(sizeof(uint32_t) + sizeof(uint32_t));

    memcpy(result.data(), reinterpret_cast<const uint8_t*>(&baseID), sizeof(uint32_t));
    memcpy(result.data() + sizeof(uint32_t), reinterpret_cast<const uint8_t*>(&unitID), sizeof(uint32_t));

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeRepair;
    packet->size = (uint32_t)result.size();
    packet->data = new uint8_t[packet->size];
    memcpy(packet->data, result.data(), result.size() * sizeof(uint8_t));
    packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::loop() {

    time_t initial = time(0);
    time_t current = initial;

    PFSocketCommandType exitCommand = PFSocketCommandTypeUnknown;

    while (false == _threadTerminate.load()) {

        exitCommand = update();

        current = time(0);

        double diff = difftime(current, initial);

        if (diff > 1 && _playing) {

            initial = time(0);

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeHeartbeat;

            _socket->sendPacket(packet);
        }
    }

    if (_socket) {

        _socket->close();
    }

    cout << "loop terminated" << endl;

    _allowConnection = true;

    if (exitCommand == PFSocketCommandTypeUnknown) {

        return;
    }

    cout << "Reconnecting..." << endl;

    if (connect()) {

        if (callback) {

            callback(exitCommand, {});
        }
    }
    else {

        if (callback) {

            callback(PFSocketCommandTypeDisconnect, {});
        }
    }
}

PFSocketCommandType PFMultiplayerClient::update() {

    _socket->update();

    auto command = _socket->getCurrentCommand();

    if (command == PFSocketCommandTypeUnknown) {

        return command;
    }

    switch (command) {

        case PFSocketCommandTypeHeartbeat:
        case PFSocketCommandTypeUnknown:
        case PFSocketCommandTypeGetGameInfo:
            break;

        case PFSocketCommandTypeMakeRoom: {

            auto roomData = _socket->getPacketData();
            memcpy(&_port, roomData.data(), roomData.size()*sizeof(uint8_t));
            _socket->dispose();

            disconnect();

            break;
        }

        case PFSocketCommandTypeDisconnect: {

            disconnect();

            if (callback) {
                callback(PFSocketCommandTypeDisconnect, {});
            }

            break;
        }

        case PFSocketCommandTypeOk: {

            _socket->dispose();

            if (callback) {

                callback(command, {});
            }

            break;
        }

        case PFSocketCommandTypeRemoveRoom:
        case PFSocketCommandTypeLeaveRoom: {

            _socket->dispose();
            _port = DEFAULT_SERVER_PORT;

            disconnect();

            break;
        }
        case PFSocketCommandTypeFire:
        case PFSocketCommandTypeMove:
        case PFSocketCommandTypeBuild:
        case PFSocketCommandTypeLoad:
        case PFSocketCommandTypeReady:
        case PFSocketCommandTypeRooms: 
        case PFSocketCommandTypeGameInfo:
        case PFSocketCommandTypeEndGame:
        case PFSocketCommandTypeSendTurn:
        case PFSocketCommandTypeCapture:
        case PFSocketCommandTypeRepair: {

            //recive curre
            auto data = _socket->getPacketData();

            _socket->dispose();

            if (callback) {

                callback(command, data);
            }

            break;
        }
    }

    _socket->dispose();

    return command;
}

