//
//  GameLogic.hpp
//  PixFight
//
//  Created by Marcin Małysz on 08/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "Core-pch.hpp"
#include "GameUnit.hpp"
#include "GameBase.hpp"
#include "GameMap.hpp"

#include "FontRender.hpp"
#include "DrawingContext.hpp"
#include "GameAnimation.hpp"
#include "Audio.hpp"
#include "GameTimer.hpp"
#include "FontRender.hpp"
#include "GameSync.hpp"

class GameLogic final {

    static const int32_t headerID;

    //timer
    constexpr static const float MAX_FRAME_TIME = 0.01;
    float _accumulator;
    float _sec;

public:

    void *context; //platform specific context will be send as parameter in callback methods

    std::function<void(void* context)> winGameCallback;
    std::function<void(void* context)> loseGameCallback;
    
    std::function<void(void* context)> botsStartThinkCallback;
    std::function<void(void* context)> botsEndThinkCallback;

    //you may want to use this for e.g saving undo
    std::function<void(void* context)> gameUnitFinishMoveCallback;

    GameLogic(const float &screenWidth,
              const float &screenHeight,
              const std::string & path,
              const Audio *audioUnit);

    ~GameLogic() noexcept;

    GameLogic(const GameLogic & other) = delete;
    GameLogic(GameLogic && other) noexcept = delete;

    GameLogic & operator= (const GameLogic & other) = delete;
    GameLogic & operator= (GameLogic && other) noexcept = delete;

    bool createNewGame(const std::string & gamename,
                       const int & selectedTeam,
                       const int & maxplayers);
    
    bool loadGame(const std::string & loadpath);
    bool saveGame(const std::string & savepath);

    void startTurn();
    bool canEndTurn();
    void endTurn();

    //call this method in your active OpenGL context
    //only valid context is 2.0ES / 3.2Core
    void Render();

    bool loadUndo(const std::string & path);
    bool saveUndo(const std::string & path);

    float multiplyTime();

    void startAnimating();
    void stopAnimating();
    bool isAnimating() const { return _animating; } 

    float getCurrentScale() const { return _currentScale; }
    xVec2 & getDirectionVec() { return _directionVector; }

    //gameplay
    void setDirectionVec(const xVec2 & vec);
    void setCurrentScale(const float & scale);
    void touchDownAtPoint(const xVec2 & position);

    //this callback will be triggered if you select base using `touchDownAtPoint`
    std::function<void(void *, GameBase *)> baseSelectedCallback;

    //use this function after showing build menu using base recivied from `baseSelectedCallback`
    GameUnit* buildUnit(GameBase *base);
    
    void setHardAI(const bool & hardAI);

    int & getPlayerCash() { return _playerCash; }
    void setPlayerCash(const int & cash) { _playerCash = cash; } 

    std::string & getCurrentMapName() { return _currentMapName; }

    auto * getUnits() { return &_units; }
    
private:

    void think(const float & delta);
    void unitFinishMoving(GameUnit *unit);
    void unitsToMerge(GameUnit *unit);
    void unitsToFight(GameUnit *unit);
    void roadForUnit(GameUnit *unit);

    struct unitdata {
        xVec2 pos;
        int32_t TEAM_ID;
        int32_t UNIT_STATE;
        int32_t TYPE;
        int32_t SUBUNIT_SIZE;
        int32_t UNIT_UNIQUEID;
        int32_t FIND_BASEID;
        int8_t experience;
        int8_t dir;
        bool mayAttack;
    };

    struct basedata {
        xVec2 pos;
        int32_t TEAM_ID;
        int32_t BASE_STATE;
        int32_t BASE_ACTION;
        int32_t TURN_TO_UNLOCK;
        int32_t BASE_UNIQUEID;
        int32_t FIND_UNITID;
        int32_t UNIT_TO_BUILD;
    };

    struct gameheder {
        char map[256];
        int32_t CURRENT_PLAYER;
        int32_t PLAYER_CASH;
        int32_t PLAYERS_PLAYING;
        int32_t money[3];
        xVec2 mapPos;
        float scale;
    };

    bool loadLogic(const std::string & logicPath);
    bool loadMap(const std::string & mapPath);

    void setupOpenGL();
    void teardownOpenGL();

    void sortUnits();
    void proceedBotsLogic();
    
    float _screenWidth;
    float _screenHeight;
    
    std::string _rootPath;

    GameMap *_mainMap;
    std::vector<GameUnit *> _units;
    std::vector<GameBase *> _bases;
    std::vector<AIPlayer *> _bots;
    std::vector<GameAnimation *> _explosions;

    //map & rendering
    float _currentScale;
    xVec2 _directionVector;

    //time multiply
    float _timeMultiplier;

    //game state
    bool _gameLoaded;

    int _winnerID;
    int _playersPlaying;
    int _playerTeamSelected;
    int _playerCash;
    int64_t _gamecounterID;

    bool _animating;

    int _fps;

    Audio::SoundID okSound;
    
    DrawingContext *_drawingContext;
    FontRender *_fontRender;
    const Audio *_audioUnit;
    
    std::string _currentMapName;
    GameTimer *_timer;

    FontRender *_font;
    GameUnit *_selectedUnit;
    bool _hardAI;
    bool _allowRendering;
};
