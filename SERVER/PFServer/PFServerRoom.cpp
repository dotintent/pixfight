//
//  PFServerRoom.cpp
//  PFServer
//
//  Created by Marcin Małysz on 18/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "PFServerRoom.hpp"

using namespace std;

static const double kMaximumRoomTime = 60 * 5;
static const double kMaximumPlayerTimeout = 30;

PFServerRoom::PFServerRoom(uint32_t port, bool privateRoom)
: _port(port)
, _private(privateRoom)
, _status(PFRoomStatusAwaiting)
, _roomInfo({})
, _lastUpdate(time(0))
, _roomSocket(new CPassiveSocket())
, _client(nullptr)
, _connectedPlayers({})
, _currentPlayerTurn(0) {

    memset(&_roomInfo, 0, sizeof(_roomInfo));

    _roomSocket->Initialize();
    _roomSocket->SetNonblocking();

    if (!_roomSocket->Listen(nullptr, _port)) {

        cout << "Error starting room: " << _roomSocket->DescribeError() << endl;
        _roomSocket->Close();
        _status = PFRoomStatusTimeout;
    }
}

PFServerRoom::~PFServerRoom() noexcept {

    for (auto &player : _connectedPlayers) {

        player->close();
    }

    _roomSocket->Close();
}

void PFServerRoom::update() {

    bool allowsPlayers = false;

    if (_roomInfo.players > 0) {

        allowsPlayers = _roomInfo.players < _connectedPlayers.size();
    }

    //finding clients
    if (((_client = _roomSocket->Accept()) != nullptr) && (_status == PFRoomStatusAwaiting) && allowsPlayers) {

        cout << "Player connected." << endl;

        _lastUpdate = time(0);

        //make non blocking
        _client->SetNonblocking();

        //assign new player
        auto player = make_shared<PFSocketClient>(_client);

        _connectedPlayers.push_back(player);

        //start looking for other one
        player = nullptr;
    }

    auto currentTime = time(0);

    //clients code
    for (auto it = _connectedPlayers.begin(); it != _connectedPlayers.end();) {

        auto player = *it;

        player->update();
        auto remove = handlePlayer(player);

        double diff = difftime(currentTime, player->getCurrentTime());

        //player timeout or need to be removed
        if (diff > kMaximumPlayerTimeout || remove) {

            player->close();
            player->dispose();

            it = _connectedPlayers.erase(it);

            _status = remove ? PFRoomStatusAwaiting : PFRoomStatusTimeout;
        }
        else {

            ++it;
        }
    }
}

bool PFServerRoom::isValid() {

    for (auto &player : _connectedPlayers) {

        if (!player->isValid()) {

            return false;
        }
    }

    return _roomSocket->IsSocketValid();
}

bool PFServerRoom::isUnused() {

    if (_status == PFRoomStatusTimeout || _status == PFRoomStatusFinished) {

        return true;
    }

    if (_status == PFRoomStatusAwaiting) {

        time_t current = time(0);
        double diff = difftime(current, _lastUpdate);

        //Timeout
        if (diff > kMaximumRoomTime) {

            _status = PFRoomStatusTimeout;
            return false;
        }
    }

    return true;
}

bool PFServerRoom::handlePlayer(shared_ptr<PFSocketClient> &player) {

    auto command = player->getCurrentCommand();

    if (command == PFSocketCommandTypeUnknown) {

        return false;
    }

    switch (command) {

        case PFSocketCommandTypeMakeRoom:
        case PFSocketCommandTypeUnknown:
        case PFSocketCommandTypeDisconnect:
        case PFSocketCommandTypeRooms:
        case PFSocketCommandTypeOk: 
            break;

        case PFSocketCommandTypeHeartbeat: {

            player->ping();
            confirmPacket(player);

            break;
        }
        case PFSocketCommandTypeLeaveRoom: {

            confirmPacket(player);
            player->dispose();

            return true;
        }
        case PFSocketCommandTypeRemoveRoom: {

            _status = PFRoomStatusFinished;
            confirmPacket(player);

            break;
        }
        case PFSocketCommandTypeGameInfo: {

            _lastUpdate = time(0);

            auto roomData = player->getPacketData();
            memcpy(&_roomInfo, roomData.data(), sizeof(_roomInfo));

            confirmPacket(player);

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeGameInfo;
            packet->size = (uint32_t)roomData.size();
            packet->data = new uint8_t[packet->size];
            memcpy(packet->data, roomData.data(), sizeof(_roomInfo));
            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

            for (auto &local : _connectedPlayers) {

                if (local == player) {
                    continue;
                }

                local->sendPacket(packet);
            }

            break;
        }
        case PFSocketCommandTypeSendTurn: {

            _currentPlayerTurn++;

            if (_currentPlayerTurn >= _connectedPlayers.size()) {

                _currentPlayerTurn = 0;
            }

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeSendTurn;
            packet->size = sizeof(_currentPlayerTurn) / sizeof(uint8_t);
            packet->data = new uint8_t[packet->size];
            memcpy(packet->data, &_currentPlayerTurn, sizeof(_currentPlayerTurn));
            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

            for (auto &local : _connectedPlayers) {

                local->sendPacket(packet);
            }

            break;
        }
        case PFSocketCommandTypeEndGame: {

            //finish game on each side sending player who wins and close room
            auto result = player->getPacketData();
            uint32_t winnerID = 0;
            memcpy(&winnerID, result.data(), sizeof(winnerID));

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeEndGame;
            packet->size = sizeof(winnerID) / sizeof(uint8_t);
            packet->data = new uint8_t[packet->size];
            memcpy(packet->data, &winnerID, sizeof(winnerID));
            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

            for (auto &local : _connectedPlayers) {

                local->sendPacket(packet);
            }

            _status = PFRoomStatusFinished;

            break;
        }
        case PFSocketCommandTypeReady: {

            //if all players ready start game
            _lastUpdate = time(0);

            player->setReady(true);

            uint16_t playersReady = 0;

            for (auto &p : _connectedPlayers) {

                if (p->isReady()) {
                    playersReady++;
                }
            }

            auto size = _connectedPlayers.size();

            if (playersReady == size && _roomInfo.players == size) {

                _status = PFRoomStatusPlaying;

                auto packet = make_unique<PFPacket>();
                packet->type = PFSocketCommandTypeLoad;

                for (auto &p : _connectedPlayers) {

                    p->sendPacket(packet);
                }
            }

            break;
        }
        case PFSocketCommandTypeLoad: {

            //TODO: all players need to send LOAD Info before start!

            player->setLoaded(true);

            uint16_t playersLoaded = 0;

            for (auto &p : _connectedPlayers) {

                if (p->isLoaded()) {
                    playersLoaded++;
                }
            }

            auto size = _connectedPlayers.size();

            if (playersLoaded == size && _roomInfo.players == size) {

                //player loaded map send his turn info
                auto packet = make_unique<PFPacket>();
                packet->type = PFSocketCommandTypeSendTurn;
                packet->size = sizeof(_currentPlayerTurn) / sizeof(uint8_t);
                packet->data = new uint8_t[packet->size];
                memcpy(packet->data, &_currentPlayerTurn, sizeof(_currentPlayerTurn));
                packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

                for (auto &p : _connectedPlayers) {

                    p->sendPacket(packet);
                }
            }

            break;
        }
        case PFSocketCommandTypeFire: {

            //pass attacker id, attacked id and both size
            auto result = player->getPacketData();

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeFire;
            packet->size = (uint32_t)result.size();
            packet->data = new uint8_t[packet->size];
            memcpy(packet->data, result.data(), packet->size * sizeof(uint8_t));
            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

            for (auto &p : _connectedPlayers) {

                if (p == player) {
                    continue;
                }

                p->sendPacket(packet);
            }

            break;
        }
        case PFSocketCommandTypeMove: {

            //pass unit id and destination vector
            auto result = player->getPacketData();

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeMove;
            packet->size = (uint32_t)result.size();
            packet->data = new uint8_t[packet->size];
            memcpy(packet->data, result.data(), packet->size * sizeof(uint8_t));
            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

            for (auto &p : _connectedPlayers) {

                if (p == player) {
                    continue;
                }

                p->sendPacket(packet);
            }

            break;
        }
        case PFSocketCommandTypeBuild: {

            //pass base id, unit type to build

            auto result = player->getPacketData();

            auto packet = make_unique<PFPacket>();
            packet->type = PFSocketCommandTypeBuild;
            packet->size = (uint32_t)result.size();
            packet->data = new uint8_t[packet->size];
            memcpy(packet->data, result.data(), packet->size * sizeof(uint8_t));
            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

            for (auto &p : _connectedPlayers) {

                if (p == player) {
                    continue;
                }

                p->sendPacket(packet);
            }

            break;
        }
    }

    player->dispose();

    return false;
}

void PFServerRoom::confirmPacket(std::shared_ptr<PFSocketClient> &player) {

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeOk;
    player->sendPacket(packet);
}
