//
//  AIPlayer.hpp
//  PixFight
//
//  Created by Marcin Małysz on 09/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "AIObject.hpp"
#include "AITaskGenerator.hpp"

#include "GameMap.hpp"
#include "GameBase.hpp"
#include "GameUnit.hpp"

#include "EnumOptionMacro.hpp"

typedef PF_ENUM(uint32_t, UNIT_DESTROY) {

    DESTROY_NONE,
    DESTROY_FIRST,
    DESTROY_SECOND,
    DESTROY_BOTH
};

extern UNIT_DESTROY attackUnit(GameUnit *a, GameUnit *b,
                               std::vector<GameUnit *> & units,
                               std::vector<GameBase *> & bases);

class AIPlayer {

    constexpr static const int MAXUSER_ALLOWED_TOPLAY = 4;

public:

    AIPlayer(const int & playerID, GameMap *map, const std::string basePath);
    ~AIPlayer() noexcept;

    void setHardAI(const bool & hardAI) { _hardAI = hardAI; } 

    AIPlayer(const AIPlayer & other) = delete;
    AIPlayer(AIPlayer && other) noexcept = delete;

    AIPlayer & operator= (const AIPlayer & other) = delete;
    AIPlayer & operator= (AIPlayer && other) noexcept = delete;

    const int & getPlayerID() const { return _playerID; }
    AIObject *addAIObject(AIAware *object);
    void updateAI(std::vector<GameUnit *> & units, std::vector<GameBase *> & bases);
    void executeAI(std::vector<GameUnit *> & units, std::vector<GameBase *> & bases, GameMap * map);
    void removeAI(const int & unitID);
    void setMoney(const int & money) { _playerMoney = money; }
    const int & getMoney() { return _playerMoney; };
    
private:

    xVec2 farthesPositionInDirection(const xVec2 & direction, const std::vector<xVec2> & points);
    bool containsPoint(const xVec2 & p, const std::vector<xVec2> & points);
    std::vector<xVec2> getAIPoints(GameUnit* current, const int & maxlenght, std::vector<GameUnit *> & units, std::vector<GameBase *> & bases);
    bool canMove(const xVec2 & start, const xVec2 & end);
    std::vector<xVec2> getAIEnemyUnitsPos(GameUnit* current, const int & maxlenght, std::vector<GameUnit *> & units);

    int _playerID;
    int _playerMoney;
    bool _hardAI;
    
    std::string _basePath;
    GameMap *_map;
    
    AITaskGenerator *_generator;
    std::vector<AIObject *> _allObjects;

};
