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

    switch (player->getCurrentCommand()) {

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
            
            return true;
        }
        case PFSocketCommandTypeRemoveRoom: {

            break;
        }
        case PFSocketCommandTypeGameInfo: {

            break;
        }
        case PFSocketCommandTypeSendTurn: {

            break;
        }
        case PFSocketCommandTypeReceiveTurn: {

            break;
        }
        case PFSocketCommandTypeEndGame: {

            break;
        }
        case PFSocketCommandTypeReady: {

            break;
        }
        case PFSocketCommandTypeLoad: {

            break;
        }
        case PFSocketCommandTypeFire: {

        }
            break;
        case PFSocketCommandTypeMove: {

            break;
        }
        case PFSocketCommandTypeBuild: {

            break;
        }
    }

    return false;
}

void PFServerRoom::confirmPacket(std::shared_ptr<PFSocketClient> &player) {

    auto packet = make_unique<PFPacket>();
    packet->type = PFSocketCommandTypeOk;
    player->sendPacket(packet);
}
