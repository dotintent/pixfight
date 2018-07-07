//
//  GameBase.hpp
//  PixFight
//
//  Created by Marcin Małysz on 07/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "GameUnit.hpp"

class AIPlayer;

class GameBase final : public DrawableObject , public AIAware  {

    friend class AIPlayer;

public:

    GameBase(const std::string & path, const xVec2 & size, const xVec2 & tile, const int & teamID);
    ~GameBase() noexcept;

    GameBase(const GameBase & other);
    GameBase(GameBase && other) noexcept = delete;

    GameBase & operator= (const GameBase & other) = delete;
    GameBase & operator= (GameBase && other) noexcept = delete;

    void setMap(GameMap * map);
    void setPosition(const xVec2 & p);

    xVec2 getAIPosition() const;
    xVec2 & getPosition() { return _position; }
    const UNITMODE & getUnitMode() const { return _mode; }
    const BASEACTION & getBaseAction() const { return _baseAction; }
    const int & getTurnsToUnlock() const { return _turnsToUnlock; }
    const int & getUnitToBuild() const { return _unitToBuild; }

    bool isOccupated(GameUnit * unit);
    bool isSelected();
    bool mayUnlock();

    void setTeam(const int & teamID);
    void setUnitToBuild(const int & unit) { _unitToBuild = unit; }
    void setTurnsToUnlock(const int & turns) { _turnsToUnlock = turns; }
    void setBaseAction(const BASEACTION & action) { _baseAction = action; }

    //Drawable
    virtual void draw(const DrawingContext & context) override;

    //AIAware
    virtual int getUniqueID() override { return _uniqueBaseID; }
    virtual int getRequestID() override { return _requestUnitID; }
    virtual int getTeamID() override { return _teamID; }
    virtual bool isBase() override { return true; }

    virtual void setUnitMode(const UNITMODE & mode) override { _mode = mode; }
    virtual void setRequestID(const int & requestid) override { _requestUnitID = requestid; }
    virtual void setUniqueID(const int & uniqueid) override { _uniqueBaseID = uniqueid; };

private:

    Mesh2d *_mesh;
    Mesh2d *_number;

    cFGLT _baseTex;
    cFGLT _digitTex;

    GameMap *_map;

    xVec2 _AABB;
    float _radius;

    int _turnsToUnlock;
    xVec2 _outputPos;
    xVec2 _position;

    int32_t _teamID;

    UNITMODE _mode;
    BASEACTION _baseAction;

    int32_t _unitToBuild;

    int32_t _uniqueBaseID;
    int32_t _requestUnitID;
};
