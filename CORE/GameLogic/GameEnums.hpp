//
//  GameEnums.hpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "EnumOptionMacro.hpp"

typedef PF_ENUM(uint32_t, BASEACTION) {

    BASE_REPAIR,
    BASE_CAPTURED,
    BASE_BUILD,
    BASE_NONE,
    BASE_SIZE
};

typedef PF_ENUM(uint32_t, M_ORIENTATION) {

    OR_LD,
    OR_P,
    OR_PD,
    OR_L,
    OR_LG,
    OR_PG
};

typedef PF_ENUM(uint32_t, UNITMODE) {

    UNIT_NONE,
    UNIT_NOTMOVE,
    UNIT_ENDTURN,
    UNIT_LOCKED,
    UNIT_INTERFACE,
    UNIT_SIZE
};

typedef PF_ENUM(uint32_t, UNITTYPE) {

    M_INFANTRY,
    M_BAZOOKA,
    M_JEEP,
    M_LTANK,
    M_ARTILLERY,
    M_BASEUNIT,
    M_SIZE
};

