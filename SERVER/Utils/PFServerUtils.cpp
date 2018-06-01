//
//  PFServerUtils.cpp
//  PFServer
//
//  Created by Marcin Małysz on 17/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "PFServerUtils.hpp"

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

uint32_t crc32c(uint32_t crc, const uint8_t *buf, size_t len)
{
    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }
    return ~crc;
}

const uint32_t DefaultHeaderSize() {

    PFPacket packet; packet.size = 0;
    return PacketSize(packet);
}

const uint32_t PacketSize(const PFPacket &packet) {

    uint32_t size = 0;

    size += sizeof(packet.version);
    size += sizeof(packet.crcsum);
    size += sizeof(packet.type);
    size += sizeof(packet.uid);
    size += sizeof(packet.size);
    size += packet.size;

    return size;
}
