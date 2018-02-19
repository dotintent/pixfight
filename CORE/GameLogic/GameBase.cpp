//
//  GameBase.cpp
//  PixFight
//
//  Created by Marcin Małysz on 07/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameBase.hpp"
#include "glTextureLoader.hpp"

GameBase::GameBase(const std::string & path, const xVec2 & size, const xVec2 & tile, const int & teamID) {

    _map = nullptr;

    std::string basePath = path + "base.png";

    _baseTex.iTexture  = textureLoader.loadFile(basePath, GL_LINEAR, 0, GL_REPEAT, false);
    _baseTex.Width  = size.x;
    _baseTex.Height =  size.y;
    _baseTex.TileWidth = tile.x;
    _baseTex.TileHeight = tile.y;
    _baseTex.tiles_x = 5;
    _baseTex.tiles_y = 1;
    _baseTex.active = true;

    _mesh = new Mesh2d(&_baseTex);
    _mesh->setAnimation(0, 0);

    std::string digitsPath = path + "digits.png";

    _digitTex.iTexture  = textureLoader.loadFile(digitsPath, GL_LINEAR, 0, GL_REPEAT, false);
    _digitTex.Width  = 256;
    _digitTex.Height =  32;
    _digitTex.TileWidth = 20;
    _digitTex.TileHeight = 20;
    _digitTex.tiles_x = 10;
    _digitTex.tiles_y = 1;
    _digitTex.active = true;

    _number = new Mesh2d(&_digitTex);
    _number->Color = FGLFC;

    _position  = xVec2(0,0);
    _outputPos = xVec2(0,0);

    _AABB = tile;
    _AABB = _AABB * 0.5;
    _radius = tile.x > tile.y ? tile.x : tile.y;

    _teamID = teamID;

    _mesh->Color = FGLFC;
    _mesh->setAnimation(teamID, 0);

    _turnsToUnlock = 0;
    _mode = UNIT_NONE;
    _baseAction = BASE_NONE;

    _uniqueBaseID  = -1;
    _requestUnitID = -1;

    _scale = 1;
}

GameBase::~GameBase() noexcept {

    _map = nullptr;

    delete _mesh;
    _mesh = nullptr;

    delete _number;
    _number = nullptr;
}

void GameBase::setMap(GameMap * map) {

    _map = map;
}

void GameBase::draw(const DrawingContext & context) {

    _outputPos = (_position * _scale) - (_map->getBias() * _scale) - (_AABB * (1.0 - _scale));

    auto frustum = context.getFrustum();

    if (!frustum->SphereInFrustum(_outputPos.x + _AABB.x, _outputPos.y + _AABB.y, 0, _radius)) {
        return;
    }

    _mesh->pos = _outputPos;
    _mesh->scale = _scale;
    _mesh->rot = 0;

    _mesh->render(context);

    if (_turnsToUnlock != 0) {

        _number->setAnimation(_turnsToUnlock, 0);
        _number->pos = _outputPos + xVec2(30, 5);
        _number->rot = 0;
        _number->scale = _scale;

        _number->render(context);
    }
}

xVec2 GameBase::getAIPosition() const {

    return _map->selectionForPosition(_position);
}

void GameBase::setPosition(const xVec2 & p) {

    _position = p;
}

bool GameBase::isOccupated(GameUnit * unit) {

    xVec2 testPoint = unit->getRealPosition();

    return AlmostEqual(_position, testPoint);
}

bool GameBase::isSelected() {

    if ((_mode == UNIT_INTERFACE) || (_mode == UNIT_ENDTURN) || (_mode == UNIT_LOCKED)) { return false; }

    xVec2 Pos = _map->hitTest();

    return AlmostEqual(_position, Pos);
}

bool GameBase::mayUnlock() {

    return _turnsToUnlock == 0;
}

void GameBase::setTeam(const int & teamID) {

    _teamID = teamID;
    _mesh->setAnimation(teamID, 0);
}
