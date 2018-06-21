//
//  GameLogic.cpp
//  PixFight
//
//  Created by Marcin Małysz on 08/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameLogic.hpp"
#include "glTextureLoader.hpp"
#include "AIPlayer.hpp"

const int32_t GameLogic::headerID = 0x00ABCD;

GameLogic::GameLogic(const float &screenWidth,
                     const float &screenHeight,
                     const std::string & path,
                     const Audio *audioUnit)
: _accumulator(0)
, _sec(0)
, _screenWidth(screenWidth)
, _screenHeight(screenHeight)
, _rootPath(path)
, _mainMap(nullptr)
, _units({})
, _bases({})
, _bots({})
, _currentScale(1.0)
, _directionVector(0, 0)
, _timeMultiplier(4.0)
, _gameLoaded(false)
, _winnerID(0)
, _playersPlaying(0)
, _playerTeamSelected(0)
, _playerCash(0)
, _gamecounterID(0)
, _animating(false)
, _fps(0)
, _drawingContext(nullptr)
, _audioUnit(audioUnit)
, _currentMapName("")
, _timer(nullptr)
, _client() {

#ifndef __EMSCRIPTEN__
    auto fontPath = _rootPath + "Lato-Black.ttf";
    _font = new FontRender(fontPath);

    auto shaderPath = _rootPath + "fontsimple";
    _font->setupOpenGL(shaderPath);
    _font->setFontSize(30);
    _font->setFontColor(FGLRed);
#endif

    _selectedUnit = nullptr;

    winGameCallback = nullptr;
    loseGameCallback = nullptr;
    botsStartThinkCallback = nullptr;
    botsEndThinkCallback = nullptr;
    gameUnitFinishMoveCallback = nullptr;
    baseSelectedCallback = nullptr;

    _hardAI = false;
    _botsThinking = false;
}

GameLogic::~GameLogic() noexcept {

    clearUndo();

    this->teardownOpenGL();

#ifndef __EMSCRIPTEN__
    if (_font) {

        delete _font;
        _font = nullptr;
    }
#endif

}

bool GameLogic::createNewGame(const std::string &gamename,
                              const int & selectedTeam,
                              const int & maxplayers,
                              std::shared_ptr<PFMultiplayerClient> client) {

    _client = client;

    _gameLoaded = false;

    auto okSoundPath = _rootPath + "ok.wav";
    okSound = _audioUnit->loadSound(okSoundPath);

    this->stopAnimating();
    this->teardownOpenGL();
    this->setupOpenGL();

    _playerTeamSelected = selectedTeam;
    PLAYERTEAMSELECTED = selectedTeam;

    _currentMapName = gamename;

    auto mappath = _rootPath + gamename + ".map";
    auto logicpath = _rootPath + gamename + ".lgf";

    if (!this->loadMap(mappath)) {

        std::cerr << "FATAL: no map loaded." << std::endl;
        return false;
    }

    std::cout << "MAP LOADED: " << mappath << std::endl;

    _playerCash = 200;
    _winnerID = 0;
    _playersPlaying = maxplayers;

    _mainMap->loadCamForPlayer(selectedTeam - 1);

    if (!this->loadLogic(logicpath)) {

        std::cerr << "FATAL: no logic loaded." << std::endl;
        return false;
    }

    std::cout << "LOGIC LOADED: " << logicpath << std::endl;

    for (auto actual : _units) {

        if (actual->getTeamID() == _playerTeamSelected) {

            actual->setUnitMode(UNIT_NONE);
        }
        else {

            actual->setUnitMode(UNIT_INTERFACE);
        }
    }

    for (auto base : _bases) {

        if (base->getTeamID() == _playerTeamSelected) {

            base->setUnitMode(UNIT_NONE);
        }
        else {

            base->setUnitMode(UNIT_INTERFACE);
        }
    }

    //create bots
    if (_client.lock() != nullptr) {

        AIPlayer *botPlayer = nullptr;

        for (int i = 1; i < _playersPlaying+1; ++i) {

            if (_playerTeamSelected == i) continue;

            botPlayer = new AIPlayer(i, _mainMap, _rootPath);
            botPlayer->setMoney(200);
            botPlayer->setHardAI(_hardAI);

            for (auto unit : _units) {

                if (unit->getTeamID() == botPlayer->getPlayerID()) {

                    botPlayer->addAIObject(unit);
                }
            }

            for (auto base : _bases) {

                if (base->getTeamID() == botPlayer->getPlayerID()) {

                    botPlayer->addAIObject(base);
                }
            }

            _bots.push_back(botPlayer);
        }
    }

    _gameLoaded = true;

    this->startAnimating();

    std::cout << "New game loaded" << std::endl;

    return true;
}

bool GameLogic::loadGame(const std::string & loadpath) {

    auto okSoundPath = _rootPath + "ok.wav";
    okSound = _audioUnit->loadSound(okSoundPath);

    std::fstream m_file;

    m_file.open(loadpath.c_str(), std::fstream::in|std::fstream::binary);

    if (!m_file) {

        std::cerr << "Error Durning opening file: " << loadpath << " propably file not exist" << std::endl;
        return false;
    }

    _gameLoaded = false;
    this->stopAnimating();
    this->teardownOpenGL();
    this->setupOpenGL();

    int32_t fileID;
    gameheder header;

    m_file.read((char*)&fileID, sizeof(fileID));

    if (fileID != headerID) {

        std::cerr << "Error file is not an game save file" << std::endl;
        return false;
    }

    m_file.read((char*)&header, sizeof(header));

    _playersPlaying     = header.PLAYERS_PLAYING;
    _playerTeamSelected = header.CURRENT_PLAYER;
    PLAYERTEAMSELECTED  = _playerTeamSelected;
    _playerCash         = header.PLAYER_CASH;

    _currentMapName = header.map;
    auto mappath = _rootPath + _currentMapName + ".map";

    if (!this->loadMap(mappath)) {

        std::cerr << "FATAL map does not exist" << std::endl;
        return false;
    }

    int32_t unitsCount = -1;
    int32_t basesCount = -1;

    unitdata tempUnitData;
    basedata tempBaseData;

    GameUnit *newUnit = nullptr;
    GameBase *newBase = nullptr;

    m_file.read((char*)&_gamecounterID, sizeof(_gamecounterID));
    m_file.read((char*)&unitsCount, sizeof(unitsCount));

    for (int uc = 0; uc < unitsCount; ++uc) {

        m_file.read((char*)&tempUnitData, sizeof(tempUnitData));

        GameUnit::unitspec temp;

        switch (tempUnitData.TYPE) {

            case M_INFANTRY: {

                newUnit = new GameUnit(_rootPath,
                                       "infantry",
                                       "infantryatt",
                                       tempUnitData.pos,
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       tempUnitData.TEAM_ID);

                newUnit->setBasicAnimation(6, 0.15);
                newUnit->setMap(_mainMap);
                newUnit->setSize(tempUnitData.SUBUNIT_SIZE);
                newUnit->setUnitType(M_INFANTRY);
                newUnit->setMoveLenght(UNITS_RATINGS[M_INFANTRY][2]);

                temp.Attack = UNITS_RATINGS[M_INFANTRY][0];
                temp.Guard  = UNITS_RATINGS[M_INFANTRY][1];

                newUnit->setStatistic("Infantry", temp);

                newUnit->setUniqueID(tempUnitData.UNIT_UNIQUEID);
                newUnit->setRequestID(tempUnitData.FIND_BASEID);
                newUnit->setMayAttack(tempUnitData.mayAttack);
                newUnit->setUnitMode((UNITMODE)tempUnitData.UNIT_STATE);
                newUnit->setUnitOrientation((M_ORIENTATION)tempUnitData.dir);

                _units.push_back(newUnit);
            }
                break;

            case M_BAZOOKA:{

                newUnit = new GameUnit(_rootPath,
                                       "bazooka",
                                       "bazookaatt",
                                       tempUnitData.pos,
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       tempUnitData.TEAM_ID);

                newUnit->setBasicAnimation(6, 0.15);
                newUnit->setMap(_mainMap);
                newUnit->setSize(tempUnitData.SUBUNIT_SIZE);
                newUnit->setUnitType(M_BAZOOKA);
                newUnit->setMoveLenght(UNITS_RATINGS[M_BAZOOKA][2]);

                temp.Attack = UNITS_RATINGS[M_BAZOOKA][0];
                temp.Guard  = UNITS_RATINGS[M_BAZOOKA][1];

                newUnit->setStatistic("BazookaMan", temp);
                newUnit->setUniqueID(tempUnitData.UNIT_UNIQUEID);
                newUnit->setRequestID(tempUnitData.FIND_BASEID);
                newUnit->setMayAttack(tempUnitData.mayAttack);
                newUnit->setUnitMode((UNITMODE)tempUnitData.UNIT_STATE);
                newUnit->setUnitOrientation((M_ORIENTATION)tempUnitData.dir);

                _units.push_back(newUnit);
            }
                break;

            case M_JEEP: {

                newUnit = new GameUnit(_rootPath,
                                       "jeep",
                                       "jeepturret",
                                       tempUnitData.pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       tempUnitData.TEAM_ID);

                newUnit->setMap(_mainMap);
                newUnit->setSize(tempUnitData.SUBUNIT_SIZE);
                newUnit->setUnitType(M_JEEP);
                newUnit->setMoveLenght(UNITS_RATINGS[M_JEEP][2]);

                temp.Attack = UNITS_RATINGS[M_JEEP][0];
                temp.Guard  = UNITS_RATINGS[M_JEEP][1];

                newUnit->setStatistic("HumVee", temp);

                newUnit->setUniqueID(tempUnitData.UNIT_UNIQUEID);
                newUnit->setRequestID(tempUnitData.FIND_BASEID);
                newUnit->setMayAttack(tempUnitData.mayAttack);
                newUnit->setUnitMode((UNITMODE)tempUnitData.UNIT_STATE);
                newUnit->setUnitOrientation((M_ORIENTATION)tempUnitData.dir);

                _units.push_back(newUnit);
            }
                break;

            case M_LTANK: {

                newUnit = new GameUnit(_rootPath,
                                       "tank",
                                       "turret",
                                       tempUnitData.pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(512,512),
                                       xVec2(80,80),
                                       xVec2(-6,-6),
                                       tempUnitData.TEAM_ID);

                newUnit->setMap(_mainMap);
                newUnit->setSize(tempUnitData.SUBUNIT_SIZE);
                newUnit->setUnitType(M_LTANK);
                newUnit->setMoveLenght(UNITS_RATINGS[M_LTANK][2]);

                temp.Attack = UNITS_RATINGS[M_LTANK][0];
                temp.Guard  = UNITS_RATINGS[M_LTANK][1];

                newUnit->setStatistic("LightTank", temp);

                newUnit->setUniqueID(tempUnitData.UNIT_UNIQUEID);
                newUnit->setRequestID(tempUnitData.FIND_BASEID);
                newUnit->setMayAttack(tempUnitData.mayAttack);
                newUnit->setUnitMode((UNITMODE)tempUnitData.UNIT_STATE);
                newUnit->setUnitOrientation((M_ORIENTATION)tempUnitData.dir);

                _units.push_back(newUnit);
            }
                break;

            case M_ARTILLERY: {

                newUnit = new GameUnit(_rootPath,
                                       "tank",
                                       "artilleryturret",
                                       tempUnitData.pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(4,-2),
                                       tempUnitData.TEAM_ID);

                newUnit->setAnimation(8, 0.1);
                newUnit->setMap(_mainMap);
                newUnit->setSize(tempUnitData.SUBUNIT_SIZE);
                newUnit->setUnitType(M_ARTILLERY);
                newUnit->setMoveLenght(UNITS_RATINGS[M_ARTILLERY][2]);
                newUnit->setAttackLenght(5);

                temp.Attack = UNITS_RATINGS[M_ARTILLERY][0];
                temp.Guard  = UNITS_RATINGS[M_ARTILLERY][1];

                newUnit->setStatistic("Artillery", temp);
                newUnit->setUniqueID(tempUnitData.UNIT_UNIQUEID);
                newUnit->setRequestID(tempUnitData.FIND_BASEID);
                newUnit->setMayAttack(tempUnitData.mayAttack);
                newUnit->setUnitMode((UNITMODE)tempUnitData.UNIT_STATE);

                newUnit->setUnitOrientation((M_ORIENTATION)tempUnitData.dir);

                _units.push_back(newUnit);
            }
                break;

            default:
                break;
        }

    }

    m_file.read((char*)&basesCount, sizeof(basesCount));

    for (int bc = 0; bc < basesCount; ++bc) {

        m_file.read((char*)&tempBaseData, sizeof(tempBaseData));

        newBase = new GameBase(_rootPath,
                               xVec2(512.0,64.0),
                               xVec2(64.0,64.0),
                               tempBaseData.TEAM_ID);

        newBase->setMap(_mainMap);
        newBase->setPosition(tempBaseData.pos);
        newBase->setUniqueID(tempBaseData.BASE_UNIQUEID);
        newBase->setRequestID(tempBaseData.FIND_UNITID);
        newBase->setUnitToBuild(tempBaseData.UNIT_TO_BUILD);
        newBase->setTurnsToUnlock(tempBaseData.TURN_TO_UNLOCK);
        newBase->setBaseAction((BASEACTION)tempBaseData.BASE_ACTION);
        newBase->setUnitMode((UNITMODE)tempBaseData.BASE_STATE);

        _bases.push_back(newBase);
    }

    m_file.close();

    _currentScale = header.scale;

    _mainMap->setScale(_currentScale);
    _mainMap->move(header.mapPos);

    //create bots
    AIPlayer *botPlayer = nullptr;

    for (int i = 1; i < _playersPlaying+1; ++i) {

        if (_playerTeamSelected == i) continue;

        botPlayer = new AIPlayer(i, _mainMap, _rootPath);
        botPlayer->setMoney(200);
        botPlayer->setHardAI(_hardAI);

        for (auto unit : _units) {

            if (unit->getTeamID() == botPlayer->getPlayerID()) {

                botPlayer->addAIObject(unit);
            }
        }

        for (auto base : _bases) {

            if (base->getTeamID() == botPlayer->getPlayerID()) {

                botPlayer->addAIObject(base);
            }
        }

        _bots.push_back(botPlayer);
    }

    for (int c = 0; c < _bots.size(); ++c) {

        botPlayer = _bots[c];
        botPlayer->setMoney(header.money[c]);
    }

    _gameLoaded = true;
    this->startAnimating();

    return true;
}

bool GameLogic::saveGame(const std::string & savepath) {

    this->stopAnimating();

    std::fstream m_file;

    m_file.open(savepath.c_str(), std::fstream::out|std::fstream::binary);

    if (!m_file) {

        std::cerr << "Error saving file: " << savepath << " propably no disk space" << std::endl;
        return false;
    }

    m_file.write((const char*)&headerID, sizeof(headerID));

    gameheder header;

    header.PLAYERS_PLAYING = _playersPlaying;
    header.CURRENT_PLAYER = _playerTeamSelected;
    header.PLAYER_CASH = _playerCash;
    header.mapPos = _mainMap->getBias();
    header.scale = _mainMap->getCurrendScale();
    strcpy(header.map, _currentMapName.c_str());
    memset(&header.money[0], 0, sizeof(header.money));

    for (int i = 0; i <_bots.size(); ++i) {

        auto ai = _bots[i];
        header.money[i] = ai->getMoney();
    }

    m_file.write((const char*)&header, sizeof(header));
    m_file.write((const char*)&_gamecounterID, sizeof(_gamecounterID));

    int32_t unitsCount = (int32_t)_units.size();
    int32_t basesCount = (int32_t)_bases.size();

    unitdata tempUnitData;
    basedata tempBaseData;

    m_file.write((const char*)&unitsCount, sizeof(unitsCount));

    for (int uc = 0; uc < unitsCount; ++uc) {

        auto currentUnit = _units[uc];

        tempUnitData.pos = currentUnit->getRealPosition();
        tempUnitData.dir = (int8_t)currentUnit->getOrientation();
        tempUnitData.TEAM_ID = currentUnit->getTeamID();
        tempUnitData.UNIT_STATE = (int32_t)currentUnit->getUnitMode();
        tempUnitData.TYPE = (int32_t)currentUnit->getUnitType();
        tempUnitData.SUBUNIT_SIZE = (int32_t)currentUnit->getSize();
        tempUnitData.UNIT_UNIQUEID = currentUnit->getUniqueID();
        tempUnitData.FIND_BASEID = currentUnit->getRequestID();
        tempUnitData.mayAttack = currentUnit->canAttack();
        GameUnit::unitspec tmpspec = currentUnit->getStats();
        tempUnitData.experience = 0;

        m_file.write((const char*)&tempUnitData, sizeof(tempUnitData));
    }

    m_file.write((const char*)&basesCount, sizeof(basesCount));

    for (int bc = 0; bc < basesCount; ++bc) {

        auto currentBase = _bases[bc];

        tempBaseData.pos = currentBase->getPosition();
        tempBaseData.TEAM_ID = currentBase->getTeamID();
        tempBaseData.BASE_STATE = (int32_t)currentBase->getUnitMode();
        tempBaseData.BASE_ACTION = (int32_t)currentBase->getBaseAction();
        tempBaseData.TURN_TO_UNLOCK = currentBase->getTurnsToUnlock();
        tempBaseData.BASE_UNIQUEID = currentBase->getUniqueID();
        tempBaseData.FIND_UNITID = currentBase->getRequestID();
        tempBaseData.UNIT_TO_BUILD = currentBase->getUnitToBuild();

        m_file.write((const char*)&tempBaseData, sizeof(tempBaseData));
    }

    m_file.close();

    this->startAnimating();

    return true;
}

void GameLogic::startTurn() {

    //update data and do some unit tests
    std::cout << "Start turn" << std::endl;

    std::vector<int> unitsToRemove;

    for (auto u : _units) {

        if (u->getTeamID() == _playerTeamSelected) {

            u->setUnitMode(UNIT_NONE);
            u->setMayAttack(true);
        }
        else {

            u->setUnitMode(UNIT_INTERFACE);
        }
    }

    // check if bases are not locked

    _playerCash += 100;

    for (auto base : _bases) {

        bool shoulddecrase = false;

        for (auto u : _units) {

            if (u->getRequestID() == base->getUniqueID()) {

                if (u->getTeamID() == _playerTeamSelected) {

                    shoulddecrase = true;
                    u->setUnitMode(UNIT_LOCKED);
                    break;
                }
            }
        }

        if ((base->getTeamID() == _playerTeamSelected) || (shoulddecrase == true)) {

            //bonus per base
            _playerCash += 100;

            auto turns = base->getTurnsToUnlock();

            if (turns > 0) {

                base->setTurnsToUnlock(--turns); //decrase

                if (base->mayUnlock()) {

                    switch (base->getBaseAction()) {
                        case BASE_BUILD : {

                            for (auto u : _units) {

                                if (u->getUniqueID() == base->getRequestID()) {

                                    u->setUnitMode(UNIT_NONE);
                                    u->setRequestID(-1);
                                    break;
                                }
                            }

                            base->setRequestID(-1);
                            base->setUnitMode(UNIT_NONE);
                        }
                            break;

                        case BASE_REPAIR : {

                            for (auto u : _units) {

                                if (u->getUniqueID() == base->getRequestID()) {

                                    u->setUnitMode(UNIT_NONE);
                                    u->setRequestID(-1);

                                    u->setSize(10);
                                }
                            }

                            base->setRequestID(-1);
                            base->setUnitMode(UNIT_NONE);
                        }
                            break;

                        case BASE_CAPTURED : {

                            for (auto u : _units) {

                                if (u->getUniqueID() == base->getRequestID()) {

                                    u->setUnitMode(UNIT_NONE);
                                    u->setRequestID(-1);

                                    base->setTeam(u->getTeamID());

                                    unitsToRemove.push_back(u->getUniqueID());
                                }
                            }

                            base->setRequestID(-1);
                            base->setUnitMode(UNIT_NONE);
                        }
                            break;

                        default:
                            break;
                    }
                }
                else {

                    base->setUnitMode(UNIT_LOCKED);
                }
            }
            else {

                base->setUnitMode(UNIT_NONE);
            }

        }
        else {

            base->setUnitMode(UNIT_INTERFACE);
        }

    }

    //remove units if needed
    for (auto it = _units.begin(); it != _units.end();) {

        if (unitsToRemove.empty()) {
            break;
        }

        auto rit = find(unitsToRemove.begin(), unitsToRemove.end(), (*it)->getUniqueID());

        if (rit == unitsToRemove.end()) {
            ++it;
            continue;
        }

        unitsToRemove.erase(rit);

        delete *it;

        it = _units.erase(it);
    }

    _mainMap->cleanRoad();
    _mainMap->cleanAStar();

    //find winner

    int PLAYERS_LEFT = 0; //at end must be 1
    int CURRENT_WIN_ID = -1;
    int BASES_COUNT = 0;

    for (int i = 0; i < _playersPlaying; ++i) {

        CURRENT_WIN_ID = (i+1); //USERS ID 1-4

        for (auto base : _bases) {

            if (base->getTeamID() == CURRENT_WIN_ID) {
                PLAYERS_LEFT++;

                _winnerID = CURRENT_WIN_ID;
                break;
            }
        }
    }

    for (int i = 0; i < _playersPlaying; ++i) {

        CURRENT_WIN_ID = (i+1); //USERS ID 1-4

        for (auto base : _bases) {

            if (base->getTeamID() == CURRENT_WIN_ID) {

                BASES_COUNT++;
            }
            else {

                BASES_COUNT = 0;
            }
        }

        if (BASES_COUNT == _bases.size()) break;
    }

    //more than 1 player have bases?

    if (PLAYERS_LEFT != 1) {

        _winnerID = 0; //reset still no winner
    }

    if (BASES_COUNT != _bases.size()) {

        _winnerID = 0; //you not own all bases
    }

    if (_winnerID == 0) {
        return;
    }

    if (auto client = _client.lock()) {

        client->sendWinnerID(_winnerID);
        return;
    }

    if (_winnerID == _playerTeamSelected) {

        if (winGameCallback) {

            winGameCallback(this->context);
        }
    }
    else {

        if (loseGameCallback) {

            loseGameCallback(this->context);
        }
    }
}

bool GameLogic::canEndTurn() {

    for (auto u : _units) {

        if (u->isMoving()) {

            return false;
        }
    }

    return true;
}

void GameLogic::endTurn() {

    clearUndo();

    _selectedUnit = nullptr;
    _mainMap->cleanRoad();

    for (auto u : _units) {

        if (u->getTeamID() == _playerTeamSelected) {

            u->setUnitMode(UNIT_ENDTURN);
        }
    }

    for (auto base : _bases) {

        if (base->getTeamID() == _playerTeamSelected) {

            base->setUnitMode(UNIT_ENDTURN);
        }
    }

    if (auto client = _client.lock()) {

        client->endTurn();

        return;
    }

    if (_winnerID == 0) {

        if (botsStartThinkCallback) {
            botsStartThinkCallback(this->context);
        }

        _botsThinking = true;

#ifdef __EMSCRIPTEN__

    #ifdef __EMSCRIPTEN_PTHREADS__

    std::thread thread([&]() {

        this->proceedBotsLogic();
    });

    thread.detach();

    #else

        this->proceedBotsLogic();

    #endif

#else

    std::thread thread([&]() {

        this->proceedBotsLogic();

        std::cout << "Thread: " << std::this_thread::get_id() << " finished!" << std::endl;
    });

    thread.detach();

#endif

    }
}

void GameLogic::Render() {

    if (!_gameLoaded) {
        return;
    }

    //sync code if needed
    syncMutex.lock();
    while (!syncQueue.empty()) {

        auto callback = syncQueue.front();
        callback(this->context, this);
        syncQueue.pop();
    }
    syncMutex.unlock();

    //sort units
    this->sortUnits();

    //timer (fixed timestamp)

    _timer->update();
    _sec = _timer->getElapsedSeconds();
    _sec = xMath::max((xF32)0.0, _sec);

    _accumulator += xMath::max(_sec, MAX_FRAME_TIME);
    _accumulator = xMath::clamp(_accumulator, (xF32)1.0, (xF32)0.0);

    //proceed thinking
    while (_accumulator >= MAX_FRAME_TIME) {

        if (_animating) {

            this->think(MAX_FRAME_TIME * _timeMultiplier);
        }

        _directionVector = (_directionVector * 95.0) * MAX_FRAME_TIME;
        if(fabs(_directionVector.x) < 0.4) _directionVector.x = 0.0f;
        if(fabs(_directionVector.y) < 0.4) _directionVector.y = 0.0f;

        _accumulator -= MAX_FRAME_TIME;
    }

    if (_botsThinking) {

        for (auto u : _units) {

            if (u->isMoving()) {

                _mainMap->move(u->getCurrentPosition() - xVec2(_screenWidth / 2.0, _screenHeight / 2.0));
                break;
            }
        }
    }

    //render (iOS disclaimer: we do not need retina scale here so no x2)
    glViewport(0, 0, _screenWidth, _screenHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto tileShader = _drawingContext->getTileShader();

    tileShader->begin();

    _mainMap->draw(*_drawingContext);

    for (auto base : _bases) {

        base->setScale(_mainMap->getCurrendScale());
        base->draw(*_drawingContext);
    }

    for (auto unit : _units) {

        unit->setScale(_mainMap->getCurrendScale());
        unit->draw(*_drawingContext);
    }

    xVec2 bias = _mainMap->getBias();

    for (auto anim : _explosions) {

        anim->setScale(_mainMap->getCurrendScale());
        anim->Draw(*_drawingContext, bias);
    }

    if (_sec == 0) {
        _sec = 1;
    }

    //draw some statistics

#ifndef __EMSCRIPTEN__
    float sx = 2.0 / _screenWidth;
    float sy = 2.0 / _screenHeight;

    int fps = int(1.0 / _sec);

    std::stringstream ss;
    ss << "FPS: " << fps;
    auto infostr = ss.str();

    ss.str("");
    ss << "MONEY: " << _playerCash;
    auto cashstr = ss.str();

    _font->begin();

    _font->setFontSize(30);
    _font->setFontColor(FGLRed);
    _font->drawText(infostr.c_str(), 1.7, -1.9, sx, sy);

    _font->setFontSize(40);
    _font->setFontColor(FGLBlack);
    _font->drawText(cashstr.c_str(), -1.96, -1.9, sx, sy);

    _font->end();
#endif

    _drawingContext->unbindVertexArray();
}

void GameLogic::think(const float & delta) {

    _mainMap->move(_directionVector.x, _directionVector.y, MAX_FRAME_TIME);

    auto lambda = [&](GameUnit *unit) {

        this->unitFinishMoving(unit);
    };

    for (auto u : _units) {

        u->think(delta, lambda);
    }

    for (auto anim : _explosions){

        anim->update(delta);
    }
}

void GameLogic::unitFinishMoving(GameUnit *unit) {

    std::cout << "unitFinishMoving: " << unit->getTeamID() << std::endl;

    for (auto base : _bases) {

        if (base->isOccupated(unit)) {

            if(base->getTeamID() == unit->getTeamID()){ //same team

                if (unit->getSize() < 10) { //unit damaged repair

                    if ((unit->getUnitType() != M_INFANTRY) && (unit->getUnitType() != M_BAZOOKA)) { //if not capture unit

                        base->setTurnsToUnlock(1);
                        base->setBaseAction(BASE_REPAIR);
                        base->setUnitMode(UNIT_LOCKED);
                        unit->setUnitMode(UNIT_LOCKED);
                        unit->setRequestID(base->getUniqueID());
                        base->setRequestID(unit->getUniqueID());
                    }
                }
            }
            else { //enemy unit

                if ((unit->getUnitType() == M_INFANTRY) || (unit->getUnitType() == M_BAZOOKA)) {

                    base->setTurnsToUnlock(2);
                    base->setBaseAction(BASE_CAPTURED);
                    base->setUnitMode(UNIT_LOCKED);
                    unit->setUnitMode(UNIT_LOCKED);
                    unit->setRequestID(base->getUniqueID());
                    base->setRequestID(unit->getUniqueID());
                }
            }

            break;
        }
    }

    action = true;

    if (gameUnitFinishMoveCallback) {

        gameUnitFinishMoveCallback(this->context);
    }
}

bool GameLogic::canUndo() {

    return !_undos.empty();
}

void GameLogic::undo() {

    syncToMainLoop([&](void *, GameLogic*){

        this->loadUndo();
    });
}

void GameLogic::loadUndo() {

    if (_undos.empty()) {
        return;
    }

    auto undo = _undos.back();

    for (auto it = _units.begin(); it != _units.end(); ++it) {

        delete *it;
        *it = nullptr;
    }

    for (auto it = _bases.begin(); it != _bases.end(); ++it) {

        delete *it;
        *it = nullptr;
    }

    _playerCash = undo.cash;

    _units.clear();
    _bases.clear();

    _units.insert(_units.end(), undo.units.begin(), undo.units.end());
    _bases.insert(_bases.end(), undo.bases.begin(), undo.bases.end());

    _undos.pop_back();
}

void GameLogic::saveUndo() {

    moveUndo currentUndo;

    currentUndo.cash = _playerCash;
    currentUndo.bases.clear();
    currentUndo.units.clear();

    for (auto u : _units) {

        currentUndo.units.push_back(new GameUnit(*u));
    }

    for (auto b : _bases) {

        currentUndo.bases.push_back(new GameBase(*b));
    }

    _undos.push_back(currentUndo);

    if (_undos.size() > MAX_UNDO_ALLOWED) {

        auto undo = _undos.front();

        for (auto unit : undo.units) {

            delete unit;
        }

        for (auto base : undo.bases) {

            delete base;
        }

        _undos.erase(_undos.begin());
    }
}

void GameLogic::clearUndo() {

    for (auto undo : _undos) {

        for (auto unit : undo.units) {

            delete unit;
        }

        for (auto base : undo.bases) {

            delete base;
        }

        undo.units.clear();
        undo.bases.clear();
    }

    _undos.clear();
}

float GameLogic::multiplyTime() {

    _timeMultiplier *= 2.0;

    if (_timeMultiplier > 4.0) {
        _timeMultiplier = 2.0;
    }

    return _timeMultiplier;
}

void GameLogic::startAnimating() {

    _animating = true;
}

void GameLogic::stopAnimating() {

    _animating = false;
}

bool GameLogic::loadLogic(const std::string & logicPath) {

    std::fstream m_file;

    m_file.open(logicPath.c_str(), std::fstream::in|std::fstream::binary);

    if (!m_file) {

        std::cerr << "Error Durning loading file: " << logicPath << " File not found or data corrupte" << std::endl;

        m_file.close();

        return false;
    }

    GameUnit::unitspec temp;
    int32_t teamID = 0;
    int32_t unitID = 0;

    xVec2 pos = xVec2(0,0);
    UNITTYPE type;

    int32_t unitsSize = 0;
    int32_t basesSize = 0;

    GameUnit *newUnit = nullptr;
    GameBase *newBase = nullptr;

    m_file.read((char*)&unitsSize, sizeof(int32_t));

    for (int i = 0; i < unitsSize; ++i) {

        m_file.read((char*)&teamID, sizeof(int32_t));
        m_file.read((char*)&pos, sizeof(xVec2));
        m_file.read((char*)&unitID, sizeof(int32_t));

        type = (UNITTYPE)unitID;

        switch (type) {

            case M_INFANTRY: {

                newUnit = new GameUnit(_rootPath,
                                       "infantry",
                                       "infantryatt",
                                       pos,
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       teamID);

                newUnit->setBasicAnimation(6, 0.15);
                newUnit->setMap(_mainMap);
                newUnit->setSize(10);
                newUnit->setUnitType(M_INFANTRY);
                newUnit->setMoveLenght(UNITS_RATINGS[M_INFANTRY][2]);

                temp.Attack = UNITS_RATINGS[M_INFANTRY][0];
                temp.Guard  = UNITS_RATINGS[M_INFANTRY][1];

                newUnit->setStatistic("Infantry", temp);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_BAZOOKA: {

                newUnit = new GameUnit(_rootPath,
                                       "bazooka",
                                       "bazookaatt",
                                       pos,
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       teamID);

                newUnit->setBasicAnimation(6, 0.15);
                newUnit->setMap(_mainMap);
                newUnit->setSize(10);

                newUnit->setUnitType(M_BAZOOKA);
                newUnit->setMoveLenght(UNITS_RATINGS[M_BAZOOKA][2]);

                temp.Attack = UNITS_RATINGS[M_BAZOOKA][0];
                temp.Guard  = UNITS_RATINGS[M_BAZOOKA][1];

                newUnit->setStatistic("BazookaMan", temp);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_JEEP: {

                newUnit = new GameUnit(_rootPath,
                                       "jeep",
                                       "jeepturret",
                                       pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(0,0),
                                       teamID);

                newUnit->setMap(_mainMap);
                newUnit->setSize(10);
                newUnit->setUnitType(M_JEEP);
                newUnit->setMoveLenght(UNITS_RATINGS[M_JEEP][2]);

                temp.Attack = UNITS_RATINGS[M_JEEP][0];
                temp.Guard  = UNITS_RATINGS[M_JEEP][1];

                newUnit->setStatistic("HumVee", temp);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_LTANK: {

                newUnit = new GameUnit(_rootPath,
                                       "tank",
                                       "turret",
                                       pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(512,512),
                                       xVec2(80,80),
                                       xVec2(-6,-6),
                                       teamID);

                newUnit->setMap(_mainMap);
                newUnit->setSize(10);
                newUnit->setUnitType(M_LTANK);
                newUnit->setMoveLenght(UNITS_RATINGS[M_LTANK][2]);

                temp.Attack = UNITS_RATINGS[M_LTANK][0];
                temp.Guard  = UNITS_RATINGS[M_LTANK][1];

                newUnit->setStatistic("LightTank", temp);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            case M_ARTILLERY: {

                newUnit = new GameUnit(_rootPath,
                                       "tank",
                                       "artilleryturret",
                                       pos,
                                       xVec2(256,512),
                                       xVec2(64,64),
                                       xVec2(512,512),
                                       xVec2(64,64),
                                       xVec2(4,-2),
                                       teamID);

                newUnit->setAnimation(8, 0.1);
                newUnit->setMap(_mainMap);
                newUnit->setSize(10);
                newUnit->setUnitType(M_ARTILLERY);
                newUnit->setMoveLenght(UNITS_RATINGS[M_ARTILLERY][2]);
                newUnit->setAttackLenght(5);

                temp.Attack = UNITS_RATINGS[M_ARTILLERY][0];
                temp.Guard  = UNITS_RATINGS[M_ARTILLERY][1];
               
                newUnit->setStatistic("Artillery", temp);
                newUnit->setUniqueID(GAME_IDCOUNTER++);

                _units.push_back(newUnit);
            }
                break;

            default:
                break;
        }

    }

    m_file.read((char*)&basesSize, sizeof(int32_t));

    for(int ib = 0; ib < basesSize; ++ib) {

        m_file.read((char*)&teamID, sizeof(int32_t));
        m_file.read((char*)&pos, sizeof(xVec2));

        newBase = new GameBase(_rootPath,
                               xVec2(512.0,64.0),
                               xVec2(64.0,64.0),
                               teamID);

        newBase->setMap(_mainMap);
        newBase->setPosition(pos);
        newBase->setUniqueID(GAME_IDCOUNTER++);

        _bases.push_back(newBase);
    }

    m_file.close();

    return true;
}

bool GameLogic::loadMap(const std::string & mapPath) {

    _mainMap = new GameMap(_rootPath);

    if (!_mainMap->load(mapPath)) {

        std::cerr << "Could not load map: " << mapPath << std::endl;
        return false;
    }

    int rx = ((int)(_screenWidth / 32.0)) + 3;
    int ry = ((int)(_screenHeight / 72.0)) + 1;

    xVec2 size = _mainMap->getMapSize();

    float maxS = 1.0;

    std::cout << "[INFO] Render offset: (" << rx <<"," << ry << ")" << std::endl;

    _mainMap->changeRenderOffset(rx, ry);
    _mainMap->allowScaling(true);

    if (size.x < rx) {

        maxS = (rx / size.x);

        _mainMap->setMinMaxScale(1.0, maxS);

        this->setCurrentScale(maxS);
        _mainMap->allowScaling(false);
    }
    else {

        _mainMap->setMinMaxScale(0.75, maxS);

        this->setCurrentScale(maxS);
    }

    _mainMap->loadCamForPlayer(0);

    return true;
}

void GameLogic::setCurrentScale(const float & scale) {

    _currentScale = scale;

    if (_mainMap) {
        _mainMap->setScale(_currentScale);
    }
}

void GameLogic::setDirectionVec(const xVec2 & vec) {

    _directionVector = vec;
}

void GameLogic::touchDownAtPoint(const xVec2 & position) {

    if (_botsThinking) {
        return;
    }

    if (_gameLoaded == false) {
        return;
    }

    bool unitSelected = false;
    bool clean = false;
    static bool readyToMove = false;
    static xVec2 lastTouchedPoint = xVec2(0,0);

    auto touch = position;

    for (auto u : _units) {

        if (u->isMoving()) {

            u->blink();
            return;
        }
    }

    _mainMap->testPoint(touch);

    if (_selectedUnit == nullptr) {

        GameUnit *unit = nullptr;

        for (int a = 0; a < _units.size(); ++a) {

            unit = _units[a];

            if (unit->isActive()) {

                _audioUnit->playSound(okSound);

                unit->setFindID(a);
                _mainMap->selectTile(touch);

                unitSelected = true;

                _selectedUnit = unit;
                this->roadForUnit(unit);
                break;
            }
        }
    }
    else {

        if (_selectedUnit && unitSelected == false) {

            GameUnit *unit = nullptr;

            for (int a = 0; a < _units.size(); ++a) {

                unit = _units[a];

                if (unit->isActive()) {

                    _audioUnit->playSound(okSound);

                    unit->setFindID(a);
                    _mainMap->selectTile(touch);

                    _selectedUnit = unit;
                    this->roadForUnit(unit);
                    break;
                }
            }
        }

        unitSelected = true;

        auto unit = _selectedUnit;

        bool findUnit = false;

        xVec2 testVec = _mainMap->hitTest();
        xVec2 unitVec = xVec2(0,0);

        GameUnit *current = nullptr;

        //unlock all awaitng units
        for (auto u : _units) {

            if (u == unit) continue;

            if (!current) {

                unitVec = u->getRealPosition();

                if (AlmostEqual(testVec, unitVec)) { //test hit point for selection

                    current = u;
                    findUnit = true;
                }
            }

        }

        if (findUnit) {

            if (current->getTeamID() != unit->getTeamID()) {

                if (_mainMap->canAttackAtTestPoint()) {

                    saveUndo();

                    //cleanup old explosion
                    for (auto it = _explosions.begin(); it != _explosions.end(); ) {

                        if ((*it)->end()) {

                            delete *it;
                            it = _explosions.erase(it);
                        }
                        else {

                            ++it;
                        }
                    }

                    auto unitPos = unit->getUnitPosition();
                    auto currentPos = current->getUnitPosition();

                    auto destroy = attackUnit(unit, current, _units, _bases);

                    if (auto client = _client.lock()) {

                        uint32_t unitIDA = unit->getUniqueID();
                        uint32_t unitIDB = current->getUniqueID();
                        uint32_t sizeA = std::max(unit->getSize(), 0.0f);
                        uint32_t sizeB = std::max(current->getSize(), 0.0f);

                        client->sendFireCommand(unitIDA, unitIDB, sizeA, sizeB);
                    }

                    if (unit->getUnitMode() == UNIT_NOTMOVE) {
                        unit->setUnitMode(UNIT_ENDTURN);
                    }
                    else {
                        unit->setMayAttack(false);
                    }

                    auto path = _rootPath + "explode.png";

                    switch (destroy) {
                        case DESTROY_FIRST: {

                            auto explosionL = new GameAnimation(path, unitPos, xVec2(512,64), xVec2(64,64), 8, 0.1);
                            _explosions.push_back(explosionL);
                            explosionL->begin();

                            auto it = find(_units.begin(), _units.end(), unit);
                            delete *it;
                            _units.erase(it);

                        }
                            break;
                        case DESTROY_SECOND: {

                            auto explosionR = new GameAnimation(path, currentPos, xVec2(512,64), xVec2(64,64), 8, 0.1);
                            _explosions.push_back(explosionR);
                            explosionR->begin();

                            auto it = find(_units.begin(), _units.end(), current);
                            delete *it;
                            _units.erase(it);
                        }
                            break;
                        case DESTROY_BOTH: {

                            auto explosionL = new GameAnimation(path, unitPos, xVec2(512,64), xVec2(64,64), 8, 0.1);
                            _explosions.push_back(explosionL);
                            explosionL->begin();

                            auto explosionR = new GameAnimation(path, currentPos, xVec2(512,64), xVec2(64,64), 8, 0.1);
                            _explosions.push_back(explosionR);
                            explosionR->begin();

                            auto itA = find(_units.begin(), _units.end(), unit);
                            delete *itA;
                            _units.erase(itA);

                            auto itB = find(_units.begin(), _units.end(), current);
                            delete *itB;
                            _units.erase(itB);
                        }
                            break;
                        case DESTROY_NONE:
                        default:
                            break;
                    }

                }

                clean = true;
            }
            else {

                readyToMove = false;

                _mainMap->cleanRoad();
                _selectedUnit = nullptr;

                GameUnit *cunit = nullptr;

                for(int a = 0; a < _units.size(); ++a) {

                    cunit = _units[a];

                    if (cunit->isActive()) {

                        cunit->setFindID(a);
                        _mainMap->selectTile(touch);
                        unitSelected = true;

                        _selectedUnit = unit;
                        this->roadForUnit(cunit);
                        break;
                    }
                }
            }
        }
        else {

            if (readyToMove && _mainMap->isPointEqual(lastTouchedPoint, touch)) {

                saveUndo();
                unit->makeMove();

                if (auto client = _client.lock()) {

                    client->moveUnitCommand(unit->getUniqueID(), touch.x, touch.y);
                }

                //set our unit that its cannot move anymore in this turn
                unit->setUnitMode(UNIT_NOTMOVE);

                if (!unit->canAttack()) {

                    unit->setUnitMode(UNIT_ENDTURN);
                }

                if (unit->getUnitType() == M_ARTILLERY) unit->setMayAttack(false);
                clean = true;
            }
            else {

                if (_mainMap->selectEndPoint(touch)) {

                    lastTouchedPoint = touch;
                    readyToMove = true;
                }
                else {

                    _mainMap->cleanAStar();
                    clean = true;
                }
            }
        }

        if (clean) {

            readyToMove = false;
            _mainMap->cleanRoad();
            _selectedUnit = nullptr;
        }
    }

    //bases

    if (!unitSelected) { //None of unit was selected

        GameBase *foundBase = nullptr;

        for (auto base : _bases) {

            if (base->isSelected()) {
                foundBase = base;
                break;
            }
        }

        if (foundBase != nullptr) {

            if (foundBase->getTeamID() == _playerTeamSelected) { //Our Base show view

                _audioUnit->playSound(okSound);

                if (baseSelectedCallback) {

                    baseSelectedCallback(this->context, foundBase);
                }
            }

        } else {

            _mainMap->cleanAStar();
        }
    }
}

void GameLogic::unitsToMerge(GameUnit *current) {

    if (current == nullptr) return;

    xVec2 selectPoint   = _mainMap->selectionForPosition(current->getRealPosition());
    TTile **currMap     = _mainMap->getMap();
    xVec2 mapMAX        = _mainMap->getMapSize();

    int STEP = 1;
    int a = 0, b = 0;

    a = selectPoint.x - STEP * 2;
    b = selectPoint.y;

    int ax = 0;
    int ay = 0;
    xVec2 testPoint;
    xVec2 testPoint2;

    for (int f = 0; f < STEP*2+1; ++f) {

        ax = a + (f*2);
        ay = b;

        if ((ax < 0) || (ax >= mapMAX.x)) continue;

        testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

        if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

            //but if its not locked test if there is a unit on it so we can skip it
            for (auto a : _units) {

                testPoint2 = a->getRealPosition();

                if (AlmostEqual(testPoint, testPoint2)) {

                    if (a->getTeamID() == current->getTeamID()) {

                        currMap[ax][ay].CompareHEX = true;
                    }
                }
            }
        }
    }

    int Versor = (((int)selectPoint.x % 2) ? 1 : -1);
    int d = 0;
    int e = 0;
    int g = 0;

    for (int c = 1; c < STEP+1; ++c) { //up and down so much time as step count

        d++;

        if (!(d%2)) e++;

        for (int f = 0; f < STEP*2+1-c; ++f) {

            ax = a + (f*2) + c;
            ay = b = selectPoint.y-e*Versor;

            if ((ax < 0)||(ax >= mapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapMAX.y)) continue;

            testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

            if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

                //but if its not locked test if there is a unit on it so we can skip it
                for (auto a : _units) {

                    testPoint2 = a->getRealPosition();

                    if (AlmostEqual(testPoint, testPoint2)) {

                        if (a->getTeamID() == current->getTeamID()) {

                            currMap[ax][ay].CompareHEX = true;
                        }
                    }
                }
            }
        }

        if ((d%2)) g++;

        //bottom test
        for (int f = 0; f < STEP*2+1-c; ++f) {

            ax = a + (f*2) + c;
            ay = b = selectPoint.y+g*Versor;

            if ((ax < 0)||(ax >= mapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapMAX.y)) continue;

            testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

            if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

                //but if its not locked test if there is a unit on it so we can skip it
                for (auto a : _units) {

                    testPoint2 = a->getRealPosition();

                    if (AlmostEqual(testPoint, testPoint2)) {

                        if (a->getTeamID() == current->getTeamID()) {

                            currMap[ax][ay].CompareHEX = true;
                        }
                    }
                }
            }
        }
    }

    current->setFindID(-1);
}

void GameLogic::unitsToFight(GameUnit *current) {

    if (current == nullptr) return;
    if (current->canAttack() == false) return;

    xVec2 selectPoint   = _mainMap->selectionForPosition(current->getRealPosition());
    TTile **currMap     = _mainMap->getMap();
    xVec2 mapMAX        = _mainMap->getMapSize();

    int STEP = current->getAttackLenght();
    int a = 0, b = 0;

    a = selectPoint.x - STEP * 2;
    b = selectPoint.y;

    int ax = 0;
    int ay = 0;
    xVec2 testPoint;
    xVec2 testPoint2;

    for (int f = 0; f < STEP*2+1; ++f) {

        ax = a + (f*2);
        ay = b;

        if ((ax < 0) || (ax >= mapMAX.x)) continue;

        testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

        if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

            //but if its not locked test if there is a unit on it so we can skip it
            for (auto a : _units) {

                testPoint2 = a->getRealPosition();

                if (AlmostEqual(testPoint, testPoint2)) {

                    if (a->getTeamID() != current->getTeamID()) {

                        currMap[ax][ay].AttackHEX = true;
                    }
                }
            }
        }
    }

    int Versor = (((int)selectPoint.x % 2) ? 1 : -1);
    int d = 0;
    int e = 0;
    int g = 0;

    for (int c = 1; c < STEP+1; ++c) { //up and down so much time as step count

        d++;

        if (!(d%2)) e++;

        for (int f = 0; f < STEP*2+1-c; ++f) {

            ax = a + (f*2) + c;
            ay = b = selectPoint.y-e*Versor;

            if ((ax < 0)||(ax >= mapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapMAX.y)) continue;

            testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

            if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

                //but if its not locked test if there is a unit on it so we can skip it
                for (auto a : _units) {

                    testPoint2 = a->getRealPosition();

                    if (AlmostEqual(testPoint, testPoint2)) {

                        if (a->getTeamID() != current->getTeamID()) {

                            currMap[ax][ay].AttackHEX = true;
                        }
                    }
                }
            }
        }

        if ((d%2)) g++;

        //bottom test
        for (int f = 0; f < STEP*2+1-c; ++f) {

            ax = a + (f*2) + c;
            ay = b = selectPoint.y+g*Versor;

            if ((ax < 0)||(ax >= mapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapMAX.y)) continue;

            testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

            if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

                //but if its not locked test if there is a unit on it so we can skip it
                for (auto a : _units) {

                    testPoint2 = a->getRealPosition();

                    if (AlmostEqual(testPoint, testPoint2)) {

                        if (a->getTeamID() != current->getTeamID()) {

                            currMap[ax][ay].AttackHEX = true;
                        }
                    }
                }
            }
        }
    }

    this->unitsToMerge(current);
}

void GameLogic::roadForUnit(GameUnit *current) {

    if (current == nullptr) return;

    if (current->getUnitMode() == UNIT_NOTMOVE) {
        this->unitsToFight(current);
        return;
    }

    //Step 2 set Road for all hex that are not locked and not occuped by any other unit

    xVec2 selectPoint   = _mainMap->selectionForPosition(current->getRealPosition());
    TTile **currMap     = _mainMap->getMap();
    xVec2 mapMAX        = _mainMap->getMapSize();

    int STEP = current->getMoveLenght();
    int a =0, b=0;

    //Fill first line not need to specify if its even or not
    a = selectPoint.x - STEP * 2;
    b = selectPoint.y;

    int ax = 0;
    int ay = 0;
    xVec2 testPoint;
    xVec2 testPoint2;
    bool nBreak = false;

    for (int f = 0; f < STEP*2+1; f++) {

        ax = a + (f*2);
        ay = b;

        if ((ax < 0) || (ax >= mapMAX.x)) continue; //out of map?

        testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

        if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

            //but if its not locked test if there is a unit on it so we can skip it
            for (auto a : _units) {

                testPoint2 = a->getRealPosition();

                if (AlmostEqual(testPoint, testPoint2)) {

                    currMap[ax][ay].UnitLock = true;
                    nBreak = true;
                    break;
                }
            }

            if ((current->getUnitType() != M_INFANTRY) && (current->getUnitType() != M_BAZOOKA)) {

                for (auto base : _bases) {

                    testPoint2 = base->getPosition();

                    if (AlmostEqual(testPoint, testPoint2) && (base->getTeamID() != _playerTeamSelected)) {

                        currMap[ax][ay].UnitLock = true;
                        nBreak = true;
                        break;
                    }
                }
            }
        }

        if (nBreak) {
            nBreak = false;
            continue;
        }

        currMap[ax][ay].isRoad = true;
    }

    int Versor = (((int)selectPoint.x % 2) ? 1 : -1);
    int d = 0;
    int e = 0;
    int g = 0;

    for (int c = 1; c < STEP+1; c++) { //up and down so much time as step count

        d++;
        nBreak = false;

        if (!(d%2)) e++;

        for (int f = 0; f < STEP*2+1-c; f++) {

            ax = a + (f*2) + c;
            ay = b = selectPoint.y-e*Versor;

            if ((ax < 0) || (ax >= mapMAX.x)) continue; //out of map?
            if ((ay < 0) || (ay >= mapMAX.y)) continue;

            testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

            if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

                //but if its not locked test if there is a unit on it so we can skip it
                for (auto a : _units) {

                    testPoint2 = a->getRealPosition();

                    if (AlmostEqual(testPoint, testPoint2)) {

                        currMap[ax][ay].UnitLock = true;
                        nBreak = true;
                        break;
                    }
                }

                if ((current->getUnitType() != M_INFANTRY) && (current->getUnitType() != M_BAZOOKA)) {

                    for (auto base : _bases) {

                        testPoint2 = base->getPosition();

                        if (AlmostEqual(testPoint, testPoint2) && (base->getTeamID() != _playerTeamSelected)) {

                            currMap[ax][ay].UnitLock = true;
                            nBreak = true;
                            break;
                        }
                    }
                }
            }

            if(nBreak){
                nBreak = false;
                continue;
            }

            currMap[ax][ay].isRoad = true;
        }

        nBreak = false;

        if ((d%2)) g++;

        //bottom test
        for (int f = 0; f < STEP*2+1-c; f++) {

            ax = a + (f*2) + c;
            ay = b = selectPoint.y+g*Versor;

            if ((ax < 0) || (ax >= mapMAX.x)) continue; //out of map?
            if ((ay < 0) || (ay >= mapMAX.y)) continue;

            testPoint = _mainMap->positionForSelection(xVec2(ax, ay));

            if (!FLT_EQUAL(testPoint.x, 0) && !FLT_EQUAL(testPoint.y, 0)) { //if this point its locked we not need to test it

                //but if its not locked test if there is a unit on it so we can skip it
                for (auto a : _units) {

                    testPoint2 = a->getRealPosition();

                    if (AlmostEqual(testPoint, testPoint2)) {

                        currMap[ax][ay].UnitLock = true;
                        nBreak = true;
                        break;
                    }
                }

                if ((current->getUnitType() != M_INFANTRY) && (current->getUnitType() != M_BAZOOKA)) {

                    for (auto base : _bases) {

                        testPoint2 = base->getPosition();

                        if (AlmostEqual(testPoint, testPoint2) && (base->getTeamID() != _playerTeamSelected)) {

                            currMap[ax][ay].UnitLock = true;
                            nBreak = true;
                            break;
                        }
                    }
                }

            }

            if(nBreak){
                nBreak = false;
                continue;
            }

            currMap[ax][ay].isRoad = true;
        }
    }

    //last step temporary lock all hex that are outside the pool we already made.
    _mainMap->lockRoad();
    this->unitsToFight(current);
}

void GameLogic::buildNewUnitFromBase(GameBase *base, int unitId,  int remainingCash) {

    saveUndo();

    if (auto client = _client.lock()) {

        client->buildUnitCommand(base->getUniqueID(), unitId);
    }

    base->setUnitToBuild(unitId);
    setPlayerCash(remainingCash);
    buildUnit(base);
    base = nullptr;
}

GameUnit* GameLogic::buildUnit(GameBase *base) {

    xVec2 pos = base->getPosition();
    int teamID = base->getTeamID();
    GameUnit *newUnit = nullptr;
    GameUnit::unitspec temp;

    UNITTYPE mTYPE = (UNITTYPE)base->getUnitToBuild();

    switch (mTYPE) {
        case M_INFANTRY: {

            newUnit = new GameUnit(_rootPath,
                                   "infantry",
                                   "infantryatt",
                                   pos,
                                   xVec2(512,512),
                                   xVec2(64,64),
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(0,0),
                                   teamID);

            newUnit->setBasicAnimation(6, 0.15);
            newUnit->setMap(_mainMap);
            newUnit->setSize(10);
            newUnit->setUnitType(M_INFANTRY);
            newUnit->setMoveLenght(UNITS_RATINGS[M_INFANTRY][2]);

            temp.Attack = UNITS_RATINGS[M_INFANTRY][0];
            temp.Guard  = UNITS_RATINGS[M_INFANTRY][1];

            newUnit->setStatistic("Infantry", temp);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_BAZOOKA: {

            newUnit = new GameUnit(_rootPath,
                                   "bazooka",
                                   "bazookaatt",
                                   pos,
                                   xVec2(512,512),
                                   xVec2(64,64),
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(0,0),
                                   teamID);

            newUnit->setBasicAnimation(6, 0.15);
            newUnit->setMap(_mainMap);
            newUnit->setSize(10);

            newUnit->setUnitType(M_BAZOOKA);
            newUnit->setMoveLenght(UNITS_RATINGS[M_BAZOOKA][2]);

            temp.Attack = UNITS_RATINGS[M_BAZOOKA][0];
            temp.Guard  = UNITS_RATINGS[M_BAZOOKA][1];

            newUnit->setStatistic("BazookaMan", temp);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_JEEP: {

            newUnit = new GameUnit(_rootPath,
                                   "jeep",
                                   "jeepturret",
                                   pos,
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(0,0),
                                   teamID);

            newUnit->setMap(_mainMap);
            newUnit->setSize(10);
            newUnit->setUnitType(M_JEEP);
            newUnit->setMoveLenght(UNITS_RATINGS[M_JEEP][2]);

            temp.Attack = UNITS_RATINGS[M_JEEP][0];
            temp.Guard  = UNITS_RATINGS[M_JEEP][1];

            newUnit->setStatistic("HumVee", temp);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_LTANK: {

            newUnit = new GameUnit(_rootPath,
                                   "tank",
                                   "turret",
                                   pos,
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(512,512),
                                   xVec2(80,80),
                                   xVec2(-6,-6),
                                   teamID);

            newUnit->setMap(_mainMap);
            newUnit->setSize(10);
            newUnit->setUnitType(M_LTANK);
            newUnit->setMoveLenght(UNITS_RATINGS[M_LTANK][2]);

            temp.Attack = UNITS_RATINGS[M_LTANK][0];
            temp.Guard  = UNITS_RATINGS[M_LTANK][1];

            newUnit->setStatistic("LightTank", temp);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        case M_ARTILLERY:{

            newUnit = new GameUnit(_rootPath,
                                   "tank",
                                   "artilleryturret",
                                   pos,
                                   xVec2(256,512),
                                   xVec2(64,64),
                                   xVec2(512,512),
                                   xVec2(64,64),
                                   xVec2(4,-2),
                                   teamID);

            newUnit->setAnimation(8, 0.1);
            newUnit->setMap(_mainMap);
            newUnit->setSize(10);
            newUnit->setUnitType(M_ARTILLERY);
            newUnit->setMoveLenght(UNITS_RATINGS[M_ARTILLERY][2]);
            newUnit->setAttackLenght(5);

            temp.Attack = UNITS_RATINGS[M_ARTILLERY][0];
            temp.Guard  = UNITS_RATINGS[M_ARTILLERY][1];

            newUnit->setStatistic("Artillery", temp);
            newUnit->setUniqueID(GAME_IDCOUNTER++);

            _units.push_back(newUnit);
        }
            break;

        default:
            break;
    }

    if (newUnit == nullptr) {
        return nullptr;
    }

    base->setBaseAction(BASE_BUILD);
    base->setTurnsToUnlock(1);
    base->setRequestID(newUnit->getUniqueID());
    newUnit->setRequestID(base->getUniqueID());

    newUnit->setUnitMode(UNIT_LOCKED);
    base->setUnitMode(UNIT_LOCKED);

    return newUnit;
}

void GameLogic::setHardAI(const bool & hardAI){

    _hardAI = hardAI;

    for (auto ai : _bots) {

        ai->setHardAI(_hardAI);
    }
}

void GameLogic::setupOpenGL() {

    auto shaderPath = _rootPath + "tile";
    _drawingContext = new DrawingContext(shaderPath, _screenWidth, _screenHeight);
    _timer = getPlatformTimerInstance();

    _drawingContext->calculateFrustum();
}

void GameLogic::teardownOpenGL() {

    if (_mainMap) {
        delete _mainMap;
        _mainMap = nullptr;
    }

    if (_bases.empty() == false) {

        for (auto b : _bases) {
            delete b;
        }

        _bases.clear();
        std::vector<GameBase *>().swap(_bases);
    }

    if (_units.empty() == false) {

        for (auto u : _units) {
            delete u;
        }

        _units.clear();
        std::vector<GameUnit *>().swap(_units);
    }

    if (_bots.empty() == false) {

        for (auto bo : _bots) {
            delete bo;
        }

        _bots.clear();
        std::vector<AIPlayer *>().swap(_bots);
    }

    if (_explosions.empty() == false) {

        for (auto e : _explosions) {
            delete e;
        }

        _explosions.clear();
        std::vector<GameAnimation *>().swap(_explosions);
    }

    if (_drawingContext) {
        delete _drawingContext;
        _drawingContext = nullptr;
    }

    if (_timer) {
        delete _timer;
        _timer = nullptr;
    }

    textureLoader.releaseTextures();

    //clear queue
    std::queue<std::function<void(void*, GameLogic*)>> empty;
    std::swap(syncQueue, empty);
}

void GameLogic::sortUnits() {

    auto comparator = [](GameUnit *a, GameUnit *b) {

        auto p1 = a->getRealPosition();
        auto p2 = b->getRealPosition();

        return p1.y < p2.y;
    };

    sort(_units.begin(), _units.end(), comparator);
}

void GameLogic::proceedBotsLogic() {

    for (auto bot : _bots) {

        bot->updateAI(_units, _bases);
        bot->executeAI(_units, _bases, _mainMap);
    }

    //finish
    if (botsEndThinkCallback) {
        syncToMainLoop([&](void *, GameLogic*){

            botsEndThinkCallback(this->context);
        });
    }

    syncToMainLoop([&](void *, GameLogic*){

        this->_botsThinking = false;
        this->startTurn();
    });
}

void GameLogic::remoteAttackUnit(const uint32_t &unitA,
                                 const uint32_t &unitB,
                                 const uint32_t sizeA,
                                 const uint32_t sizeB) {

    syncToMainLoop([=](void *, GameLogic *sender){

        auto units = sender->getUnits();

        GameUnit *uA = nullptr;
        GameUnit *uB = nullptr;

        for (auto u : *units) {

            if (u->getUniqueID() == unitA) {
                uA = u;
                break;
            }
        }

        for (auto u : *units) {

            if (u->getUniqueID() == unitB) {
                uB = u;
                break;
            }
        }

        if (uA && uB) {

            uA->fire();
            uB->fire();

            uA->setSize(sizeA);
            uB->setSize(sizeB);

            auto path = sender->_rootPath + "explode.png";

            if (sizeA == 0) {

                auto unitPos = uA->getUnitPosition();

                auto it = find(units->begin(), units->end(), uA);
                delete *it;
                units->erase(it);

                auto explosion = new GameAnimation(path, unitPos, xVec2(512,64), xVec2(64,64), 8, 0.1);
                sender->_explosions.push_back(explosion);
                explosion->begin();
            }

            if (sizeB == 0) {

                auto unitPos = uB->getUnitPosition();

                auto it = find(units->begin(), units->end(), uB);
                delete *it;
                units->erase(it);

                auto explosion = new GameAnimation(path, unitPos, xVec2(512,64), xVec2(64,64), 8, 0.1);
                sender->_explosions.push_back(explosion);
                explosion->begin();
            }
        }
    });
}

void GameLogic::remoteMoveUnit(const uint32_t &unitID,
                               const float &x,
                               const float &y) {

    syncToMainLoop([=](void *, GameLogic *sender){

        auto units = sender->getUnits();
        GameUnit *unit = nullptr;

        for (auto u : *units) {

            if (u->getUniqueID() == unitID) {
                unit = u;
                break;
            }
        }

        if (unit == nullptr) {
            return;
        }

        xVec2 unitPos = unit->getUnitPosition();
        xVec2 selection = sender->_mainMap->selectionForPosition(unitPos);

        sender->_mainMap->selectTile(selection);

        this->roadForUnit(unit);

        xVec2 touch(x,y);
        sender->_mainMap->selectEndPoint(touch);

        unit->makeMove();

        sender->_mainMap->cleanRoad();
        sender->_mainMap->cleanAStar();
    });
}

void GameLogic::remoteBuildUnit(const uint32_t &baseID,
                                const uint16_t &unitType) {

    syncToMainLoop([=](void *, GameLogic *sender){

        auto bases = sender->_bases;

        GameBase *base = nullptr;

        for (auto b : bases) {

            if (b->getUniqueID() == baseID) {
                base = b;
                break;
            }
        }

        if (base == nullptr) {
            return;
        }

        base->setUnitToBuild(unitType);
        buildUnit(base);
    });
}
