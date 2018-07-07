//
//  PFServerUtils.hpp
//  PFServer
//
//  Created by Marcin Małysz on 17/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "PFServerCommandType.hpp"

extern uint32_t crc32c(uint32_t crc, const uint8_t *buf, size_t len);
extern const uint32_t DefaultHeaderSize();
extern const uint32_t PacketSize(const PFPacket &packet);
