//
//  main.cpp
//  PFClient
//
//  Created by Marcin Małysz on 13/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include <map>

#include "PFMultiplayerClient.hpp"

using namespace std;

int main(int argc, const char * argv[]) {

    map<int, pair<string, int>> dummyMaps = {
        {0, make_pair("Tutorial", 2)},
        {1, make_pair("Hunt", 3)},
        {2, make_pair("Pasage", 2)},
        {3, make_pair("Waterway", 2)},
        {4, make_pair("Kingofhill", 4)},
    };

    auto client = make_unique<PFMultiplayerClient>("127.0.0.1");

    //handle
    client->callback = [&](const PFSocketCommandType &command, const std::vector<uint8_t> data) {

        cout << "Recived command: " << endl;

        switch (command) {
            case PFSocketCommandTypeOk: {

                cout << "PFSocketCommandTypeOk" << endl;
            }
                break;
            case PFSocketCommandTypeUnknown: {

                cout << "PFSocketCommandTypeUnknown" << endl;
            }
                break;
            case PFSocketCommandTypeHeartbeat: {

                cout << "PFSocketCommandTypeHeartbeat" << endl;

                break;
            }
            case PFSocketCommandTypeDisconnect: {

                cout << "PFSocketCommandTypeDisconnect" << endl;

                client->disconnect();

                break;
            }
            case PFSocketCommandTypeMakeRoom: {

                cout << "PFSocketCommandTypeMakeRoom on port: " << client->getCurrentPort() << endl;

                int map = rand() % 5;

                auto info = dummyMaps.at(map);

                PFRoomInfo roomInfo;

                strcpy(roomInfo.mapname, info.first.c_str());
                roomInfo.players = info.second;
                roomInfo.roomPort = client->getCurrentPort();

                client->setRoomInfo(roomInfo);
                client->sendRoomDetails();

                break;
            }
            case PFSocketCommandTypeLeaveRoom: {

                cout << "PFSocketCommandTypeLeaveRoom" << endl;

                break;
            }
            case PFSocketCommandTypeRemoveRoom: {

                cout << "PFSocketCommandTypeRemoveRoom" << endl;

                break;
            }
            case PFSocketCommandTypeGameInfo: {

                cout << "PFSocketCommandTypeGameInfo" << endl;

                PFRoomInfo info;

                memcpy(&info, data.data(), data.size() * sizeof(uint8_t));

                cout << "New room info: " << endl
                     << "Map: " << info.mapname << endl
                     << "Players: " << info.players << endl
                     << "Port: " << info.roomPort << endl;

                break;
            }
            case PFSocketCommandTypeSendTurn: {

                uint32_t currentPlayerTurn = 0;

                memcpy(&currentPlayerTurn, data.data(), data.size() * sizeof(uint8_t));

                cout << "PFSocketCommandTypeSendTurn" << endl;
                cout << "Current player turn: " << currentPlayerTurn << endl;

                break;
            }
            case PFSocketCommandTypeEndGame: {

                uint32_t winnnerID = 0;
                memcpy(&winnnerID, data.data(), data.size() * sizeof(uint8_t));

                cout << "PFSocketCommandTypeEndGame" << endl;
                cout << "Winner: " << winnnerID << endl;

                break;
            }
            case PFSocketCommandTypeReady: {

                cout << "PFSocketCommandTypeReady" << endl;

                break;
            }
            case PFSocketCommandTypeLoad: {

                cout << "PFSocketCommandTypeLoad" << endl;

                break;
            }
            case PFSocketCommandTypeRooms: {

                cout << "PFSocketCommandTypeRooms" << endl;

                cout << "--------------------------" << endl;

                size_t roomsSize = (data.size() * sizeof(uint8_t)) / sizeof(PFRoomInfo);

                vector<PFRoomInfo> rooms(roomsSize);

                memcpy(rooms.data(), data.data(), data.size() * sizeof(uint8_t));

                for (int i = 0; i < rooms.size(); ++i) {

                    auto room = rooms[i];

                    cout << "Room" << i << endl
                         << "Name: " << room.mapname << endl
                         << "Players: " << room.players << endl
                         << "Date: " << room.createdDate << endl
                         << "Port: " << room.roomPort << endl;
                }

                cout << "--------------------------" << endl;

                break;
            }
            case PFSocketCommandTypeFire: {

                cout << "PFSocketCommandTypeFire" << endl;

                uint32_t idA = 0;
                uint32_t idB = 0;
                uint32_t sizeA = 0;
                uint32_t sizeB = 0;

                memcpy(&idA, data.data(), sizeof(uint32_t));
                memcpy(&idB, data.data() + sizeof(uint32_t), sizeof(uint32_t));
                memcpy(&sizeA, data.data() + sizeof(uint32_t) * 2, sizeof(uint32_t));
                memcpy(&sizeB, data.data() + sizeof(uint32_t) * 3, sizeof(uint32_t));

                cout << "Unit " << idA << " attack " << idB << " size after: " << sizeA << ", " << sizeB << endl;

                break;
            }
            case PFSocketCommandTypeMove: {

                cout << "PFSocketCommandTypeMove" << endl;

                uint32_t unitID = 0;
                float posX = 0;
                float posY = 0;

                memcpy(&unitID, data.data(), sizeof(uint32_t));
                memcpy(&posX, data.data() + sizeof(uint32_t), sizeof(float));
                memcpy(&posY, data.data() + sizeof(uint32_t) + sizeof(float), sizeof(float));

                cout << "Move unit " << unitID << "position (" << posX << "," << posY << ")" << endl;

                break;
            }
            case PFSocketCommandTypeBuild: {

                cout << "PFSocketCommandTypeBuild" << endl;

                uint32_t baseID = 0;
                uint16_t unit = 0;

                memcpy(&baseID, data.data(), sizeof(uint32_t));
                memcpy(&unit, data.data() + sizeof(uint32_t), sizeof(float));

                cout << "Base: " << baseID << " build unit: " << unit << endl;

                break;
            }
        }

    };

    bool running = client->connect();

    if (!running) {

        cout << "Could not connect to server" << endl;
    }

    char cmd = '\0';

    cout << "Connected to server" << endl;

    while (running) {

        cin >> cmd;
        cout << "Choosen command: " << cmd << endl;

        if (!client->isValid()) {
            cout << "Client is no more valid" << endl;
            running = false;
        }

        switch (cmd) {

            case 'q' : {

                client->disconnect();
                running = false;

                break;
            }

            case 'j' : {

                cout << "enter room id: ";
                uint32_t roomID;
                cin >> roomID;
                client->joinRoom(roomID);

                break;
            }

            case 'm' : {

                cout << "Make private room? (0/1): ";
                bool privateRoom = false;
                cin >> privateRoom;
                client->makeRoom(privateRoom);

                break;
            }

            case 'i' : {

                cout << "Send room info";

                int map = rand() % 5;

                auto info = dummyMaps.at(map);

                PFRoomInfo roomInfo;

                strcpy(roomInfo.mapname, info.first.c_str());
                roomInfo.players = info.second;
                roomInfo.createdDate = time(0);
                roomInfo.roomPort = client->getCurrentPort();

                client->setRoomInfo(roomInfo);
                client->sendRoomDetails();

                break;
            }

            case 'k' : {

                client->leaveRoom();
                break;
            }

            case 'r' : {

                cout << "Remove room" << endl;
                client->removeRoom();

                break;
            }

            //list rooms
            case 'l': {

                cout << "List rooms" << endl;
                client->listRooms();

                break;
            }

            case 'e' : {

                cout << "End turn" << endl;
                client->endTurn();

                break;
            }

            case 'v' : {

                cout << "Set ready" << endl;
                client->setReady();

                break;
            }

            case 'b' : {

                cout << "Set loaded" << endl;
                client->setLoaded();

                break;
            }

            case '1' : {

                int randA = rand() % 10;
                int randB = rand() % 10;

                cout << "Fire A: " << randA << ", " << randB << endl;

                client->sendFireCommand(1, 3, randA, randB);

                break;
            }

            case '2' : {

                int randomUnit = rand() % 255;
                float posX = rand() % 640;
                float posY = rand() % 480;

                cout << "Moving: " << randomUnit << " to pos: " << posX << ", " << posY << endl;

                client->moveUnitCommand(randomUnit, posX, posY);

                break;
            }

            case '3' : {

                int randomBase = rand() % 255;
                uint16_t unit = rand() % 6;

                cout << "Base " << randomBase << " build unit: " << unit << endl;

                client->buildUnitCommand(randomBase, unit);

                break;
            }

            default:
                break;
        }
    }

    return 0;
}
