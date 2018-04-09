//
//  GameAnimation.cpp
//  PixFight
//
//  Created by Marcin Małysz on 15/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameAnimation.hpp"
#include "glTextureLoader.hpp"

GameAnimation::GameAnimation(const std::string & texpath,
                             const xVec2 & pos,
                             const xVec2 & size,
                             const xVec2 & tileSize,
                             const float & lenght,
                             const float & speed)
: _running(false)
, _mesh(nullptr)
, _pos(pos)
, _tx(0)
, _ty(0)
, _mx(size.x / tileSize.x)
, _my(size.y / tileSize.y)
, _animsum(lenght)
, _currentsum(0)
, _speed(speed)
, _outPos(xVec2(0,0)) {

    _Tex.iTexture = textureLoader.loadFile(texpath, GL_LINEAR, 0 , GL_REPEAT, false);
    _Tex.Width  = size.x;
    _Tex.Height = size.y;
    _Tex.TileWidth = tileSize.x;
    _Tex.TileHeight = tileSize.y;
    _Tex.tiles_x = _mx;
    _Tex.tiles_y = _my;
    _Tex.active = true;

    _mesh = new Mesh2d(&_Tex);
    _mesh->setAnimation(0, 0);

    _AABB = tileSize;
    _AABB = _AABB * 0.5;
    _radius = fmax(tileSize.x, tileSize.y);
    _accum = 0;
    _scale = 1.0;
}

GameAnimation::~GameAnimation() noexcept {

    if (_mesh) {
        delete _mesh;
        _mesh = nullptr;
    }
}

void GameAnimation::Draw(const DrawingContext & context, xVec2 bias) {

    _outPos = (_pos * _scale) - (bias * _scale) - (_AABB * (1.0 - _scale));

    auto frustum = context.getFrustum();

    if (!frustum->SphereInFrustum(_outPos.x + _AABB.x, _outPos.y + _AABB.y, 0, _radius)) return;

    _mesh->pos = _pos;
    _mesh->rot = _rot;
    _mesh->scale = _scale;

    _mesh->render(context);
}

void GameAnimation::setScale(const float & scale) {

    _scale = scale;
}

void GameAnimation::setRotation(const float & rotation) {

    _rot = rotation;
}

void GameAnimation::setPosition(const xVec2 & position) {

    _pos = position;
}

void GameAnimation::begin() {

    _running = true;
}

bool GameAnimation::end() {

    return !_running;
}

void GameAnimation::update(const float & dt) {

    if (!_running) return;

    _accum += dt;

    if (_accum >= _speed) {

        if (_currentsum <= _animsum) {

            if (_tx < _mx) {

                _tx++;
            }
            else {

                if (_ty < _my) _ty++;
                _tx = 0;
            }

            _currentsum++;
        }
        else {

            _currentsum = _animsum;
            _running = false;
        }

        _mesh->setAnimation(_tx, _ty);
        _accum = 0;
    }
}
