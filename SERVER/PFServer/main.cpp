//
//  main.cpp
//  PFServer
//
//  Created by Marcin Małysz on 13/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include <iostream>
#include <vector>
#include <functional>
#include <thread>
#include <future>

#include "PassiveSocket.h"
#include "ActiveSocket.h"

#include "PFSocketClient.hpp"
#include "PFServerRoom.hpp"

#include "PFServerCommandType.hpp"

typedef PF_ENUM(uint32_t, PFServerCommand) {

    PFServerCommandUnknown,
    PFServerCommandCreateRoom,
    PFServerCommandRoomList,
    PFServerCommandDisconnectClient
};

using namespace std;

PFServerCommand resolveCommand(const shared_ptr<PFSocketClient> &client);

int main(int argc, const char * argv[]) {

    vector<shared_ptr<PFSocketClient>> connections;
    vector<shared_ptr<PFServerRoom>> rooms;
    
    CPassiveSocket socket;
    CActiveSocket *pClient = nullptr;

    socket.Initialize();
    socket.SetNonblocking();

    if (!socket.Listen("0.0.0.0", DEFAULT_SERVER_PORT)) {

        cout << "Error starting server: " << socket.DescribeError() << endl;
        socket.Close();
        exit(0);
    }

    bool running = true;

    cout << "Staring server" << endl;

    while (running) {

        if (!socket.IsSocketValid()) {

            cout << "Internal server error: " << socket.DescribeError() << "shutting down..." << endl;

            running = false;
            continue;
        }

        //finding clients
        if ((pClient = socket.Accept()) != nullptr)
        {
            cout << "Client connected." << endl;

            //make non blocking
            pClient->SetNonblocking();

            //assign new client
            auto client = make_shared<PFSocketClient>(pClient);

            connections.push_back(client);

            //start looking for other one
            pClient = nullptr;
        }

        //clients code
        for (auto it = connections.begin(); it != connections.end();) {

            auto client = *it;

            if (!client->isValid()) {

                cout << "Client error: " << client->getLastError() << " disconnecting." << endl;

                it = connections.erase(it);
            }
            else {

                client->update();

                //not sending or receiving check result
                if (!client->isSending() && !client->isReceiving()) {

                    auto command = client->getCurrentCommand();

                    if (command == PFSocketCommandTypeUnknown) {

                        ++it;
                        continue;
                    }

                    cout << "Server command: " << command << endl;

                    switch (resolveCommand(client)) {

                        case PFServerCommandCreateRoom: {

                            uint32_t initialPort = socket.GetServerPort();
                            uint32_t roomPort = initialPort;

                            if (rooms.empty()) {

                                roomPort++;
                            }
                            else {

                                auto lastPort = rooms.back();

                                if (lastPort->getPort() > 9000) {
                                    //lol, i want this to happen
                                    //future, plans do not allow more clients to connect
                                }

                                roomPort = lastPort->getPort() + 1;
                            }

                            bool isPrivate = false;

                            auto data = client->getPacketData();
                            memcpy(&isPrivate, data.data(), sizeof(bool));

                            auto room = make_shared<PFServerRoom>(roomPort, isPrivate);

                            rooms.push_back(room);

                            //send new room info to client
                            auto packet = make_unique<PFPacket>();
                            packet->type = PFSocketCommandTypeMakeRoom;
                            packet->size = sizeof(roomPort)/sizeof(uint8_t);
                            packet->data = new uint8_t[packet->size];

                            memcpy(packet->data, &roomPort, sizeof(roomPort));

                            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

                            client->sendPacket(packet);

                            client->dispose();
                        }
                            break;

                        case PFServerCommandRoomList: {

                            vector<shared_ptr<PFServerRoom>> publicRooms;

                            copy_if(rooms.begin(), rooms.end(),
                                    inserter(publicRooms, publicRooms.begin()),
                                    [](auto &room){

                                return !room->isPrivate();
                            });

                            auto packet = make_unique<PFPacket>();
                            packet->type = PFSocketCommandTypeRooms;
                            packet->size = uint32_t(sizeof(PFRoomInfo) * publicRooms.size()) / sizeof(uint8_t);
                            packet->data = new uint8_t[packet->size];

                            //fill room info
                            for (int i = 0; i < publicRooms.size(); ++i) {

                                auto room = publicRooms[i];
                                auto roomInfo = room->getRoomInfo();

                                memcpy(packet->data + i * sizeof(PFRoomInfo), &roomInfo, sizeof(PFRoomInfo));
                            }

                            packet->crcsum = crc32c(packet->crcsum, packet->data, packet->size);

                            client->sendPacket(packet);

                            client->dispose();

                        }
                            break;

                        case PFServerCommandDisconnectClient: {

                            client->close();
                            client->dispose();
                            it = connections.erase(it);
                            continue;
                        }
                            break;

                        case PFServerCommandUnknown:
                            break;
                    }
                }

                ++it;
            }
        }

        //rooms code
        for (auto it = rooms.begin(); it != rooms.end();) {

            auto room = *it;

            if (!room->isValid() || room->isUnused()) {

                cout << "Removing room: " << distance(rooms.begin(), it) << " status: " << (int)room->getStatus() << endl;

                it = rooms.erase(it);
            }
            else {

                room->update();

                ++it;
            }
        }
    }

    //close clients connection
    for (auto &client : connections) {

        client->close();
    }

    connections.clear();
    rooms.clear();

    cout << "Server closed." << endl;

    socket.Close();
    
    return 0;
}

PFServerCommand resolveCommand(const shared_ptr<PFSocketClient> &client) {

    auto command = client->getCurrentCommand();

    switch (command) {

        case PFSocketCommandTypeDisconnect: return PFServerCommandDisconnectClient;
        case PFSocketCommandTypeMakeRoom: return PFServerCommandCreateRoom;
        case PFSocketCommandTypeRooms: return PFServerCommandRoomList;
        case PFSocketCommandTypeUnknown:
        case PFSocketCommandTypeHeartbeat:
        case PFSocketCommandTypeLeaveRoom:
        case PFSocketCommandTypeRemoveRoom:
        case PFSocketCommandTypeGameInfo:
        case PFSocketCommandTypeSendTurn:
        case PFSocketCommandTypeEndGame:
        case PFSocketCommandTypeReady:
        case PFSocketCommandTypeLoad:
        case PFSocketCommandTypeFire:
        case PFSocketCommandTypeMove:
        case PFSocketCommandTypeBuild:
        case PFSocketCommandTypeOk:
            break;
    }

    return PFServerCommandUnknown;
}
