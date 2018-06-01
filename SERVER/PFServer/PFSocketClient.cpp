//
//  PFSocketClient.cpp
//  PFServer
//
//  Created by Marcin Małysz on 17/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "PFSocketClient.hpp"

using namespace std;

const uint32_t PFSocketClient::headerSize = DefaultHeaderSize();

PFSocketClient::PFSocketClient(CActiveSocket *socket)
: _socket(unique_ptr<CActiveSocket>(socket))
, _command(PFSocketCommandTypeUnknown)
, _recivedBytes(0)
, _sendBytes(0)
, _sendSize(0)
, _data({})
, _lastPing(time(0))
, _ready(false) {

}

bool PFSocketClient::isReceiving() const {

    return _recivedBytes > 0;
}

bool PFSocketClient::isSending() const {

    return _sendBytes > 0;
}

bool PFSocketClient::isValid() const {

    if (!_socket) {

        return false;
    }

    return _socket->IsSocketValid();
}

void PFSocketClient::close() {

    if (!_socket) {
        return;
    }

    _socket->Close();
}

void PFSocketClient::update() {

    if (!_socket) {
        return;
    }

    if ((_recivedBytes += _socket->Receive(MAX_PACKET)) > 0 && (_command == PFSocketCommandTypeUnknown)) {

        auto recivedData = _socket->GetData();
        auto recivedSize = _socket->GetBytesReceived();

        _recivedData.insert(_recivedData.end(), recivedData, recivedData + recivedSize);

        //we get header
        if (_recivedData.size() >= headerSize && _data.empty()) {

            //fill up header data
            memcpy(&_localHeader, _recivedData.data(), headerSize);

            //preallocate _data
            _data.resize(_localHeader.size);
        }

        //we have complete data
        if (_recivedData.size() > headerSize + _localHeader.size) {

            //copy additional data
            memcpy(_data.data(), _recivedData.data() + headerSize, _localHeader.size * sizeof(uint8_t));

            uint32_t crc32 = 0;
            crc32 = crc32c(crc32, _data.data(), _localHeader.size);

            if (_localHeader.version == PROTOCOL_VERSION && _localHeader.crcsum == crc32) {

                //assing command
                _command = _localHeader.type;
            }

            //reset receiving
            _recivedData.clear();
            _recivedBytes = 0;
        }
    }

    _sendBytes += _socket->GetBytesSent();

    if (_sendBytes >= _sendSize) {
        _sendBytes = 0;
    }
}

std::vector<uint8_t> PFSocketClient::getPacketData() const {

    return _data;
}

void PFSocketClient::dispose() {

    _data.clear();
    _command = PFSocketCommandTypeUnknown;
}

void PFSocketClient::sendPacket(const std::unique_ptr<PFPacket> &packet) {

    _sendSize = PacketSize(*packet);

    //header
    auto headerSize = _sendSize - packet->size;
    auto headerData = reinterpret_cast<uint8_t*>(packet.get());

    _sendBytes += _socket->Send(headerData, headerSize);

    auto dataSize = packet->size;
    auto data = packet->data;

    _sendBytes += _socket->Send(data, dataSize);

    if (_sendBytes >= _sendSize) {
        _sendBytes = 0;
    }
}

