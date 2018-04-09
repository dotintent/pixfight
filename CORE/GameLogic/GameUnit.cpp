//
//  GameUnit.cpp
//  PixFight
//
//  Created by Marcin Małysz on 07/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameUnit.hpp"
#include "glTextureLoader.hpp"

xVec2 unitoffset[5*6] = {
    //M_INFANTRY
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),

    //M_BAZOOKA
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),

    //M_JEEP
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),
    xVec2(0,0),

    //M_LTANK
    xVec2(-6,-6),
    xVec2(-6,-6),
    xVec2(-6,-6),
    xVec2(-6,-6),
    xVec2(-6,-6),
    xVec2(-6,-6),

    //M_ARTILLERY
    xVec2( 2,-6),
    xVec2(-5,-3),
    xVec2(-2,-5),
    xVec2( 5,-3),
    xVec2( 4,-2),
    xVec2(-4,-2)
};

GameUnit::GameUnit(const std::string & path,
                   const std::string & texName,
                   const std::string & addTextName,
                   const xVec2 & pos,
                   const xVec2 & size,
                   const xVec2 & tile,
                   const xVec2 & asize,
                   const xVec2 & atile,
                   const xVec2 & texBias,
                   const int & teamID) {

    std::stringstream ss;

    ss << path;
    ss << texName;
    ss << teamID;
    ss << ".png";

    std::string baseName = ss.str();

    _tex.iTexture = textureLoader.loadFile(baseName, GL_LINEAR, 0, GL_REPEAT, false);
    _tex.Width  = size.x;
    _tex.Height = size.y;
    _tex.TileWidth = tile.x;
    _tex.TileHeight = tile.y;
    _tex.tiles_x = 4;
    _tex.tiles_y = 6;
    _tex.active = true;

    ss.str(std::string());

    ss << path;
    ss << addTextName;
    ss << teamID;
    ss << ".png";

    baseName = ss.str();

    _addTex.iTexture  = textureLoader.loadFile(baseName, GL_LINEAR, 0, GL_REPEAT, false);
    _addTex.Width  = asize.x;
    _addTex.Height =  asize.y;
    _addTex.TileWidth = atile.x;
    _addTex.TileHeight = atile.y;
    _addTex.tiles_x = 4;
    _addTex.tiles_y = 6;
    _addTex.active = true;

    _mesh = new Mesh2d(&_tex);
    _addMesh = new Mesh2d(&_addTex);

    _texBias = texBias;

    _animX = 0;
    _accum = 0;
    _fireaccum = 0.0f;
    _fireanim = 0;
    _fireing = false;

    //TEMOPORARY TEAMS ID'S

    _teamID = teamID;

    _mesh->Color = FGLFC;
    _addMesh->Color = FGLFC;

    switch (_teamID) {
        case 0: _currentOR = OR_PD;
            break;
        case 1: _currentOR = OR_PD;
            break;
        case 2: _currentOR = OR_LG;
            break;
        case 3: _currentOR = OR_LD;
            break;
        case 4: _currentOR = OR_PG;
            break;
        default:
            break;
    }

    _mesh->setAnimation(_animX, _currentOR);
    _addMesh->setAnimation(_animX, _currentOR);

    //-------------------------------------------

    baseName = path + "digits.png";

    _digTex.iTexture = textureLoader.loadFile(baseName, GL_LINEAR, 0, GL_REPEAT, false);
    _digTex.Width  = 256;
    _digTex.Height =  32;
    _digTex.TileWidth = 20;
    _digTex.TileHeight = 20;
    _digTex.tiles_x = 10;
    _digTex.tiles_y = 1;
    _digTex.active = true;

    _nr = new Mesh2d(&_digTex);

    baseName = path + "redgreen.png";

    _arrowTex.iTexture = textureLoader.loadFile(baseName, GL_LINEAR, 0, GL_REPEAT, false);
    _arrowTex.Width  = 64;
    _arrowTex.Height =  16;
    _arrowTex.TileWidth = 24;
    _arrowTex.TileHeight = 16;
    _arrowTex.tiles_x = 1;
    _arrowTex.tiles_y = 1;
    _arrowTex.active = true;

    _action = new Mesh2d(&_arrowTex);

    _move = false;
    _size = 1;
    _sa = 0;
    _sb = 1;

    int fixX = (int)pos.x;
    int fixY = (int)pos.y;

    _currentPos = xVec2(fixX, fixY);
    _lastPosition = xVec2(fixX, fixY);

    _lastPoint = xVec2(0,0);
    _currentPoint = xVec2(0,0);

    _AABB = tile;
    _AABB = _AABB * 0.5;
    _radius = tile.x > tile.y ? tile.x : tile.y;

    _mode = UNIT_NONE;

    _lenghtMove = 3;
    _lenghtAttack = 1;
    _findID = -1;
    _outputPosition = xVec2(0, 0);

    _name = "";

    _uniqueUnitID = -1;
    _requestBaseID = -1;
    _pathCopy.clear();

    _mayAttack = true;
    _damaged = false;

    _animationSpeed = 0.2;
    _animationLenght = 4;

    _basicAnimationSpeed = 0.15;
    _basicAnimationLenght = 4;

    _blinking = false;
    _animLock = false;
    _val = 50;
}

GameUnit::~GameUnit() noexcept {

    _map = nullptr;

    delete _action;
    _action = nullptr;

    delete _addMesh;
    _addMesh = nullptr;

    delete _mesh;
    _mesh = nullptr;

    delete _nr;
    _nr = nullptr;
}

void GameUnit::setStatistic(const std::string  & name, const unitspec & sp) {

    _name = name;
    _actualStat = sp;
}

void GameUnit::setSize(const int & i) {

    _size = i;
    if (_size > MAXUNITSIZE) _size = MAXUNITSIZE;

    _sa = _size % 10;
    _sb = (_size - _sa) * 0.1;

    _damaged = _size < 5;
}

void GameUnit::setUnitOrientation(const M_ORIENTATION & actual) {

    _currentOR = actual;

    _mesh->setAnimation(_animX, _currentOR);
    _addMesh->setAnimation(_fireanim, _currentOR);
}

void GameUnit::moveToPoint(const xVec2 & p) {

    if ((p.x == _lastPoint.x) && (p.y == _lastPoint.y))
    {
        return;
    }
    else if (((p.x - _lastPoint.x) == 2) && (p.y == _lastPoint.y))
    {
        this->setUnitOrientation(OR_P);
    }
    else if (((p.x - _lastPoint.x) == -2) && (p.y == _lastPoint.y))
    {
        this->setUnitOrientation(OR_L);
    }
    else if (((p.x - _lastPoint.x) == 1) && (p.y == _lastPoint.y))
    {
        if ((int)_lastPoint.x % 2) {

            this->setUnitOrientation(OR_PG);
        }
        else {

            this->setUnitOrientation(OR_PD);
        }
    }
    else if (((p.x - _lastPoint.x) == -1) && (p.y == _lastPoint.y))
    {
        if ((int)_lastPoint.x % 2) {

            this->setUnitOrientation(OR_LG);
        }
        else{

            this->setUnitOrientation(OR_LD);
        }
    }
    else if (((p.x - _lastPoint.x) == 1) && (p.y < _lastPoint.y))
    {
        if ((int)_lastPoint.x % 2) {

            this->setUnitOrientation(OR_PD);
        }
        else {

            this->setUnitOrientation(OR_PG);
        }
    }
    else if (((p.x - _lastPoint.x) == -1) && (p.y < _lastPoint.y))
    {
        if ((int)_lastPoint.x % 2) {

            this->setUnitOrientation(OR_LD);
        }
        else {

            this->setUnitOrientation(OR_LG);
        }
    }
    else if (((p.x - _lastPoint.x) == 1) && (p.y > _lastPoint.y))
    {
        if ((int)_lastPoint.x % 2) {

            this->setUnitOrientation(OR_PD);
        }
        else {

            this->setUnitOrientation(OR_PG);
        }
    }
    else if (((p.x - _lastPoint.x) == -1) && (p.y > _lastPoint.y))
    {
        if ((int)_lastPoint.x % 2) {

            this->setUnitOrientation(OR_LD);
        }
        else {

            this->setUnitOrientation(OR_LG);
        }
    }

    _lastPoint = p;
}

void GameUnit::draw(const DrawingContext & context) {

    _outputPosition = (_currentPos * _scale) - (_map->getBias() * _scale) - (_AABB * (1.0 - _scale));
    _texBias = unitoffset[_type * 6 + _currentOR];

    auto frustum = context.getFrustum();

    if (!frustum->SphereInFrustum(_outputPosition.x + _AABB.x, _outputPosition.y + _AABB.y, 0, _radius)) return;

    if ((_type == M_INFANTRY) || (_type == M_BAZOOKA)) {

        if (_fireing) {

            _addMesh->pos = _outputPosition + _texBias;
            _addMesh->scale = _scale;
            _addMesh->render(context);
        }
        else {

            _mesh->pos = _outputPosition;
            _mesh->scale = _scale;
            _mesh->render(context);
        }
    }
    else {

        _mesh->pos = _outputPosition;
        _mesh->scale = _scale;
        _mesh->render(context);

        _addMesh->pos = _outputPosition + _texBias;
        _addMesh->scale = _scale;
        _addMesh->render(context);
    }

    if(_size < 10){

        _nr->setAnimation(_sa, 0);
        _nr->scale = 1.0;
        _nr->pos = xVec2(_outputPosition.x + 40.0, _outputPosition.y + 5.0);
        _nr->render(context);
    }
    else {

        _nr->setAnimation(_sb, 0);
        _nr->scale = 1.0;
        _nr->pos = xVec2(_outputPosition.x + 40.0, _outputPosition.y + 5.0);
        _nr->render(context);

        _nr->setAnimation(_sa, 0);
        _nr->pos = xVec2(_outputPosition.x + 50.0, _outputPosition.y + 5.0);
        _nr->render(context);
    }

#ifndef MAP_EDITOR
    if (_teamID == PLAYERTEAMSELECTED) {

        _action->pos = _outputPosition + xVec2(20,-4+sin(_del)*6);
        _action->scale = _scale;

        _action->render(context);
    }
#endif
}

void GameUnit::think(const float & dt, std::function<void(GameUnit *)> callback) {

    if (_mode == UNIT_NONE) {

        _action->setAnimation(0, 0);
    }
    else {

        _action->setAnimation(1, 0);
    }

    if (_mayAttack && _mode != UNIT_LOCKED) {

        _del += dt * 2.0;

        if (_del >= FLT_MAX) {
            _del = 0.0;
        }
    }
    else {

        _del = 0.0;
    }

    if (_move) {

        _accum += dt;

        if (_accum > _basicAnimationSpeed) {

            _animX++;

            if (_animX == _basicAnimationLenght) { _animX = 0; }

            _mesh->setAnimation(_animX, _currentOR);
            _accum = 0.0;
        }

        if((fabs(_currentPos.x - _lastPosition.x) <= 2) && (fabs(_currentPos.y - _lastPosition.y) <= 2)){

            if(_pathCopy.empty()){

                _move = false;
                _selected = false;
                _direction = xVec2(0,0);
                _animX = 0;

                _mesh->setAnimation(_animX, _currentOR);

                _currentPos = _lastPosition;

                if (callback) {
                    callback(this);
                }
            }
            else{

                _currentPoint = _pathCopy[0];
                _pathCopy.erase(_pathCopy.begin());

                this->moveToPoint(_currentPoint);

                _lastPosition = _map->positionForSelection(_currentPoint);
                _direction = _lastPosition - _currentPos;
            }

        }

        _currentPos += _direction * dt * 1.5;
    }

    if (_fireing) {

        _fireaccum += dt;

        if (_fireaccum > _animationSpeed) {

            _fireanim++;

            if(_fireanim == _animationLenght) {
                _fireanim = 0;
                _fireing = false;
            }

            _addMesh->setAnimation(_fireanim, _currentOR);
            _fireaccum = 0.0;
        }
    }

    if (_blinking) {

        FGLM.PingPong(_val, _animLock, 50, 1);
        float outVal = (float(_val)+50.0)/100.0f;

        _mesh->Color.a = outVal;
        _addMesh->Color.a = outVal;

        if (!_animLock && _val == 49) {

            _blinking = false;
            _mesh->Color.a  = 1.0;
            _addMesh->Color.a = 1.0;
        }
    }

}

bool GameUnit::isActive() {

    if ((_mode == UNIT_INTERFACE) || (_mode == UNIT_LOCKED) || (_mode == UNIT_ENDTURN)) {

        return false;
    }

    xVec2 Pos = _map->hitTest();

    if ((fabs(_currentPos.x - Pos.x) <= 1) && (fabs(_currentPos.y - Pos.y) <= 1)) {

        _selected = true;
    }
    else {

        _selected = false;
    }

    return _selected;
}

void GameUnit::makeMove() {

    if ((_mode != UNIT_NOTMOVE) && !_move) {

        _move = true;

        auto astar = _map->getAStar();

        if (astar.empty()) {

            _move = false;
        }
        else {

            if (_pathCopy.empty()) {

                for(int i=0; i<astar.size(); i++){

                    _pathCopy.push_back(astar[i]);
                }
            }
        }
    }
}

void GameUnit::setMap(GameMap *map) {

    if (map == nullptr) {
        std::cerr << "[ERROR] map must be set!" << std::endl;
    }

    _map = map;
}

xVec2 GameUnit::getAIPosition() {

    return _map->selectionForPosition(_currentPos);
}

xVec2 GameUnit::getUnitPosition() {

    return _outputPosition;
}

xVec2 GameUnit::getRealPosition() {

    return _currentPos;
}

void GameUnit::addExp(const float & exp) {

    _actualStat.Expirence += exp;

    if (_actualStat.Expirence > 100.0) {

        _actualStat.Expirence = 100.0;
    }
}

void GameUnit::setStats(const unitspec & sp) {

    _actualStat = sp;
}

std::string & GameUnit::getName() {

    return _name;
}

GameUnit::unitspec & GameUnit::getStats() {

    return _actualStat;
}

float GameUnit::getSize() {

    return (float)_size;
}

bool GameUnit::isMoving() {

    return _move;
}

void GameUnit::setBasicAnimation(const int & lenght, const float & speed) {

    _basicAnimationLenght = lenght;
    _basicAnimationSpeed = speed;
    _tex.tiles_x = lenght;
}

void GameUnit::setAnimation(const int & lenght, const float & speed) {

    _animationLenght = lenght;
    _animationSpeed = speed;
    _addTex.tiles_x = lenght;
}

void GameUnit::fire() {

    _fireing = true;
}

void GameUnit::blink() {

    _blinking = true;
    _animLock = true;
    _val = 50;
}

