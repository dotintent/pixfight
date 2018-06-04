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

PFMultiplayerClient::PFMultiplayerClient(const std::string &serverAddress, bool async)
: callback(nullptr)
, _serverAddress(serverAddress)
, _port(DEFAULT_SERVER_PORT)
, _socket(nullptr)
, _roomDetails({})
, _loopThread({})
, _threadTerminate(!async)
, _playing(false) {

}

PFMultiplayerClient::~PFMultiplayerClient() noexcept {

    disconnect();
}

bool PFMultiplayerClient::connect() {

    if (_serverAddress.empty()) {

        return false;
    }

    auto socket = new CActiveSocket();
    socket->Initialize();
    socket->SetNonblocking();

    uint16_t retry = 0;

    while (!socket->Open(_serverAddress.c_str(), _port)) {

        this_thread::sleep_for(chrono::milliseconds(200));

        retry++;

        if (retry > 3) {

            return false;
        }
    }

    _socket = unique_ptr<PFSocketClient>(new PFSocketClient(socket));

    _threadTerminate = false;

    //call loop from another thread
    _loopThread = thread(&PFMultiplayerClient::loop, this);
    _loopThread.detach();

    return true;
}

void PFMultiplayerClient::disconnect() {

    _threadTerminate = true;

    if (!_playing) {

        auto packet = make_unique<PFPacket>();
        packet->type = PFSocketCommandTypeDisconnect;
        _socket->sendPacket(packet);
    }

    if (_socket) {
        _socket->close();
    }
}

bool PFMultiplayerClient::joinRoom(uint32_t roomid) {

    if (_playing) {
        return false;
    }

    if (_socket) {
        _socket->dispose();
        _socket->close();
        _socket = nullptr;
    }

    _threadTerminate = true;
    _port = roomid;

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

    if (_playing) {
        return;
    }

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeReady;

    _socket->sendPacket(packet);
}

void PFMultiplayerClient::setLoaded() {

    if (_playing) {
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

void PFMultiplayerClient::loop() {

    time_t initial = time(0);
    time_t current = initial;

    while (false == _threadTerminate.load()) {

        update();

        current = time(0);

        double diff = difftime(current, initial);

        if (diff > 1) {

            initial = time(0);

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeHeartbeat;

            _socket->sendPacket(packet);
        }
    }
}

void PFMultiplayerClient::update() {

    _socket->update();

    auto command = _socket->getCurrentCommand();

    if (command == PFSocketCommandTypeUnknown) {

        return;
    }

    switch (command) {

        case PFSocketCommandTypeHeartbeat:
        case PFSocketCommandTypeUnknown:
            break;

        case PFSocketCommandTypeMakeRoom: {

            auto roomData = _socket->getPacketData();
            memcpy(&_port, roomData.data(), roomData.size()*sizeof(uint8_t));
            _socket->dispose();

            disconnect();

            if (connect()) {

                if (callback) {

                    callback(command, roomData);
                }
            }
            else {

                if (callback) {

                    callback(PFSocketCommandTypeDisconnect, roomData);
                }
            }

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
            disconnect();

            _port = DEFAULT_SERVER_PORT;

            if (connect()) {

                if (callback) {

                    callback(command, {});
                }
            }
            else {

                if (callback) {

                    callback(PFSocketCommandTypeDisconnect, {});
                }
            }

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
        case PFSocketCommandTypeSendTurn: {

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
}
