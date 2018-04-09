//
//  MapTile.h
//  PixFight
//
//  Created by Marcin Małysz on 05/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "Core-pch.hpp"

#define MAX_ACTIVE_LAYERS 2

typedef struct TTile{

    int32_t tile_x[MAX_ACTIVE_LAYERS],
            tile_y[MAX_ACTIVE_LAYERS]; //Tile

    int32_t pos_x, pos_y; //position

    bool active[MAX_ACTIVE_LAYERS];

    uint32_t Cost; // 0-8 cost of move
    bool Lock;
    bool isRoad;

    bool tempLock; //temporary lock for A star
    bool UnitLock; //for non temp lock

    bool AttackHEX;
    bool CompareHEX;

} TTile;
