//
//  GameUnit.hpp
//  PixFight
//
//  Created by Marcin Małysz on 07/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <functional>
#include "GameMap.hpp"
#include "GameEnums.hpp"
#include "AIAware.hpp"

class AIPlayer;

class GameUnit final : public DrawableObject, public AIAware {

    constexpr static const int MAXUNITSIZE = 10;
    friend class AIPlayer;

public:

    typedef struct unitspec {
        float Attack;
        float Guard;
        float Expirence;
    } unitspec;

    GameUnit(const std::string & path,
             const std::string & texName,
             const std::string & addTextName,
             const xVec2 & pos,
             const xVec2 & size,
             const xVec2 & tile,
             const xVec2 & asize,
             const xVec2 & atile,
             const xVec2 & texBias,
             const int & teamID);

    ~GameUnit() noexcept;

    GameUnit(const GameUnit & other);
    GameUnit(GameUnit && other) noexcept = delete;

    GameUnit & operator= (const GameUnit & other) = delete;
    GameUnit & operator= (GameUnit && other) noexcept = delete;

    void setStatistic(const std::string  & name, const unitspec & sp);
    void setSize(const int & i);
    void setUnitOrientation(const M_ORIENTATION & actual);
    void setMoveLenght(const int & movelenght) { _lenghtMove = movelenght; }
    void setAttackLenght(const int & attacklenght) { _lenghtAttack = attacklenght; }

    void think(const float & dt, std::function<void(GameUnit *)> callback);
    bool isActive();
    void makeMove();
    void setMap(GameMap *map);

    xVec2 getAIPosition();
    xVec2 getUnitPosition();
    xVec2 getRealPosition();
    xVec2 getCurrentPosition();

    void addExp(const float & exp);
    void setStats(const unitspec & sp);

    std::string & getName();
    unitspec & getStats();
    float getSize();
    bool canAttack() { return _mayAttack; }
    const M_ORIENTATION & getOrientation() const { return _currentOR; };

    bool isMoving();
    void setBasicAnimation(const int & lenght, const float & speed);
    void setAnimation(const int & lenght, const float & speed);
    void fire();
    void blink();

    const UNITTYPE & getUnitType() const { return _type; }
    const UNITMODE & getUnitMode() const { return _mode; }
    const int & getAttackLenght() const { return _lenghtAttack; }
    const int & getMoveLenght() const { return _lenghtMove; }

    void setUnitType(const UNITTYPE & type) { _type = type; }
    void setMayAttack(const bool & attack) { _mayAttack = attack; }

    const int & getFindID() { return _findID; }
    void setFindID(const int & findID) { _findID = findID; }

    //Drawable
    virtual void draw(const DrawingContext & context) override;

    //AIAware
    virtual int getUniqueID() override { return _uniqueUnitID; }
    virtual int getRequestID() override { return _requestBaseID; }
    virtual int getTeamID() override { return _teamID; }
    virtual bool isBase() override { return false; }

    virtual void setUnitMode(const UNITMODE & mode) override { _mode = mode; }
    virtual void setRequestID(const int & requestid) override { _requestBaseID = requestid; }
    virtual void setUniqueID(const int & uniqueid) override { _uniqueUnitID = uniqueid; };

private:

    void moveToPoint(const xVec2 & p);


private:

    xVec2 _currentPos;

    M_ORIENTATION _currentOR;

    UNITMODE _mode;
    UNITTYPE _type;

    int _lenghtMove;
    int _lenghtAttack;
    int _findID;
    int _teamID;

    int32_t _uniqueUnitID;
    int32_t _requestBaseID;

    bool _mayAttack;

    int     _animationLenght;
    float   _animationSpeed;
    int     _basicAnimationLenght;
    float   _basicAnimationSpeed;

    Mesh2d*  _mesh;
    Mesh2d*  _addMesh;
    Mesh2d*  _nr;
    Mesh2d*  _action;

    cFGLT    _tex;
    cFGLT    _addTex;
    cFGLT    _digTex;
    cFGLT    _arrowTex;

    xVec2 _lastPosition;
    xVec2 _lastPoint;
    xVec2 _currentPoint;
    xVec2 _AABB;
    xVec2 _outputPosition;
    xVec2 _direction;

    xVec2 _texBias;

    float _radius;

    bool _move;
    bool _selected;

    GameMap *_map;
    std::vector<xVec2> _pathCopy;

    int _size;
    int _sa, _sb;

    unitspec _actualStat;
    std::string _name;

    bool    _damaged;
    int     _animX;
    float   _accum;
    bool    _fireing;
    bool    _blinking;
    bool    _animLock;
    int     _fireanim;
    float   _fireaccum;
    int     _val;

    float   _distance;
    xVec2   _finalPosition;
};

