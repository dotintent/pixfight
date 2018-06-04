//
//  PFServerCommandType.h
//  PFServer
//
//  Created by Marcin Małysz on 17/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "EnumOptionMacro.hpp"

#define MAX_PACKET 4096

typedef PF_ENUM(uint32_t, PFSocketCommandType) {

    PFSocketCommandTypeUnknown = 0,
    PFSocketCommandTypeHeartbeat,
    PFSocketCommandTypeDisconnect,
    PFSocketCommandTypeMakeRoom,
    PFSocketCommandTypeLeaveRoom,
    PFSocketCommandTypeRemoveRoom,
    PFSocketCommandTypeGameInfo,
    PFSocketCommandTypeSendTurn,
    PFSocketCommandTypeEndGame,
    PFSocketCommandTypeReady,
    PFSocketCommandTypeLoad,
    PFSocketCommandTypeRooms,
    PFSocketCommandTypeFire,
    PFSocketCommandTypeMove,
    PFSocketCommandTypeBuild,
    PFSocketCommandTypeOk
};

#define PROTOCOL_VERSION 1
#define DEFAULT_SERVER_PORT 5001

class PFPacket final {
public:

    PFPacket()
    : version(PROTOCOL_VERSION)
    , crcsum(0)
    , type(PFSocketCommandTypeUnknown)
    , size(0)
    , data(nullptr) {}

    PFPacket(const PFPacket& other) = delete;
    PFPacket(PFPacket&& other) noexcept = delete;
    PFPacket& operator= (const PFPacket& other) = delete;
    PFPacket& operator= (PFPacket&& other) noexcept = delete;

    ~PFPacket() noexcept {
        if (data) { delete [] data; }
    }

    uint16_t version; // PROTOCOL_VERSION
    uint32_t crcsum; //data crc sum

    //command
    PFSocketCommandType type;

    //additional data
    uint32_t size;
    uint8_t *data;
};

typedef struct {

    char mapname[128];
    uint16_t players;
    time_t createdDate;
    uint16_t roomPort;

} PFRoomInfo;