//
//  AIPlayer.cpp
//  PixFight
//
//  Created by Marcin Małysz on 09/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "AIPlayer.hpp"
#include "GameSync.hpp"
#include "GameLogic.hpp"

static int directionVersor[7] = {0, 2, -2, 3, -3, 4,-4};
static int artilleryVersor[9] = {0, 1, -1, 2, -2, 3, -3, 4, -4};

UNIT_DESTROY attackUnit(GameUnit *a, GameUnit *b, std::vector<GameUnit *> & units, std::vector<GameBase *> & bases) {

    UNIT_DESTROY unitToDestroy = DESTROY_NONE;

    GameUnit::unitspec s1 = a->getStats();
    GameUnit::unitspec s2 = b->getStats();

    float BonusA = s1.Expirence;
    float BonusB = s2.Expirence;
    float P = 0.0;

    float TaA = 0.0;
    float TdA = 0.0;

    float TaB = 0.0;
    float TdB = 0.0;

    UNITTYPE TypeA = a->getUnitType();
    UNITTYPE TypeB = b->getUnitType();

    switch (TypeA) {
        case M_INFANTRY:
            switch (TypeB) {
                case M_INFANTRY: {}
                    break;
                case M_BAZOOKA: {}
                    break;
                case M_JEEP: {
                    TaA = -3.0;
                }
                    break;
                case M_LTANK: {
                    TaA = -3.0;
                }
                    break;
                case M_ARTILLERY: {
                    TaA = -3.0;
                }
                    break;
                default:
                    break;
            }
            break;
        case M_BAZOOKA :
            switch (TypeB) {
                case M_INFANTRY: {}
                    break;
                case M_BAZOOKA: {}
                    break;
                case M_JEEP: {
                    TaA = 2.0;
                }
                    break;
                case M_LTANK: {
                    TaA = 2.0;
                }
                    break;
                case M_ARTILLERY: {
                    TaA = 2.0;
                }
                    break;
                default:
                    break;
            }
            break;
        case M_JEEP :
            switch (TypeB) {
                case M_INFANTRY: {
                    TaB = -3.0;
                }
                    break;
                case M_BAZOOKA: {
                    TaB = 2.0;
                }
                    break;
                case M_JEEP: {
                    TaA = -6.0;
                    TaB = -6.0;
                }
                    break;
                case M_LTANK: {
                    TaA = -6.0;
                    TaB = -3.0;
                }
                    break;
                case M_ARTILLERY :{
                    TaA = -6.0;
                    TaB = -6.0;
                }
                    break;
                default:
                    break;
            }
            break;
        case M_LTANK :
            switch (TypeB) {
                case M_INFANTRY: {
                    TaB = -3.0;
                }
                    break;
                case M_BAZOOKA: {
                    TaB = 2.0;
                }
                    break;
                case M_JEEP: {
                    TaA = -3.0;
                    TaB = -6.0;
                }
                    break;
                case M_LTANK: {
                    TaA = -3.0;
                    TaB = -3.0;
                }
                    break;
                case M_ARTILLERY: {
                    TaA = -3.0;
                    TaB = -6.0;
                }
                    break;
                default:
                    break;
            }
            break;
        case M_ARTILLERY :
            switch (TypeB) {
                case M_INFANTRY: {
                    TaB = -3.0;
                }
                    break;
                case M_BAZOOKA: {
                    TaB = 2.0;
                }
                    break;
                case M_JEEP: {
                    TaA = -6.0;
                    TaB = -6.0;
                }
                    break;
                case M_LTANK: {
                    TaA = -6.0;
                    TaB = -3.0;
                }
                    break;
                case M_ARTILLERY: {
                    TaA = -6.0;
                    TaB = -6.0;
                }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    float hitCount = 0.0;
    int tempNum = 0;

    int unitsToDestroyForA = 0;
    int unitsToDestroyForB = 0;

    xVec2 Ap = a->getRealPosition();
    xVec2 Bp = b->getRealPosition();
    xVec2 Out = Ap - Bp;

    bool mayGetHit = ((a->getUnitType() == M_ARTILLERY) || (Out.mag() <= 66.0));

    if (a->getUnitType() == M_ARTILLERY) {

        if (Out.mag() <= 66.0) {
            TaA -= 2.0;
        }
    }

    xVec2 testPoint;

    for (auto b : bases) {

        testPoint = b->getPosition();

        if (AlmostEqual(Ap, testPoint)) {

            TdA += 2.0;
            break;
        }
    }

    for (auto b : bases) {

        testPoint = b->getPosition();

        if (AlmostEqual(Bp, testPoint)) {

            TdB += 2.0;
            break;
        }
    }

    int SizeB = 0;
    int SizeA = 0;

    // A PART
    SizeA = a->getSize();
    SizeB = b->getSize();

    if (mayGetHit == true) {

        a->fire();

        P = 0.05 * (((s1.Attack + TaA) - (s1.Guard + TdA))+BonusA) + 0.5;

        if (P < 0.0) P = 0.0;
        if (P > 1.0) P = 1.0;

        for (int i=0; i < SizeA; i++) {
            for (int b=0; b < 6; b++) {

                tempNum = rand() % 2;
                if(tempNum < P) hitCount++;
            }}

        unitsToDestroyForB = roundf(hitCount / 6.0);
    }

    //B PART
    mayGetHit = ((b->getUnitType() == M_ARTILLERY) || (Out.mag() <= 66.0));

    if (b->getUnitType() == M_ARTILLERY) {

        if (Out.mag() <= 66.0) {

            TaB -= 2.0;
        }
    }

    if (mayGetHit == true) {

        b->fire();

        hitCount = 0.0;

        P = 0.05 * (((s2.Attack + TaB) - (s2.Guard + TdB))+BonusB) + 0.5;

        if(P < 0.0) P = 0.0;
        if(P > 1.0) P = 1.0;

        for(int i=0; i<SizeB; i++){
            for(int b=0; b<6; b++){

                tempNum = rand() % 2;
                if(tempNum < P) hitCount++;
            }}

        unitsToDestroyForA = roundf(hitCount / 6.0);
    }

    //Expirence adding part
    auto TA = a;
    auto TB = b;

    if (unitsToDestroyForB > 0.0) { //Unit hit oppnent

        TA->addExp(0.05);
    }
    else {

        TB->addExp(0.05);
    }

    if (unitsToDestroyForA > 0.0) {

        TB->addExp(0.05);
    }//not hited enemy Guard Bonus
    else {

        TA->addExp(0.05);
    }

    //Units Removing
    if (unitsToDestroyForB >= SizeB) {

        TA->addExp(0.1);

        if (TB->getRequestID() != -1) {

            for (auto fBase : bases) {

                if (fBase->getUniqueID() == TB->getRequestID()) {

                    fBase->setBaseAction(BASE_NONE);
                    if (fBase->getUnitMode() != UNIT_INTERFACE) fBase->setUnitMode(UNIT_NONE);
                    fBase->setTurnsToUnlock(0);
                    fBase->setRequestID(-1);
                    break;
                }
            }
        }

        unitToDestroy = DESTROY_SECOND;
        TB = nullptr;
    }
    else {

        SizeB -= unitsToDestroyForB;
        TB->setSize(SizeB);
    }

    //Units Removing
    if(unitsToDestroyForA >= SizeA){

        if (TB != nullptr) {

            TB->addExp(0.1);

            if (TA->getRequestID() != -1) {

                for (auto fBase : bases) {

                    if (fBase->getUniqueID() == TA->getRequestID()) {

                        fBase->setBaseAction(BASE_NONE);
                        if (fBase->getUnitMode() != UNIT_INTERFACE) fBase->setUnitMode(UNIT_NONE);
                        fBase->setTurnsToUnlock(0);
                        fBase->setRequestID(-1);
                    }
                }
            }

            if (unitToDestroy == DESTROY_SECOND) {

                unitToDestroy = DESTROY_BOTH;
            }
            else {

                unitToDestroy = DESTROY_FIRST;
            }

            TA = nullptr;
        }
        else {

            SizeA = 1;
            TA->setSize(SizeA);
        }
    }
    else {

        SizeA -= unitsToDestroyForA;
        TA->setSize(SizeA);
    }

    return unitToDestroy;
}

AIPlayer::AIPlayer(const int & playerID, GameMap *map, const std::string basePath)
: _playerID(playerID)
, _playerMoney(0)
, _hardAI(false)
, _basePath(basePath)
, _map(map)
, _generator(new AITaskGenerator()) {

    _allObjects.clear();
}

AIPlayer::~AIPlayer() noexcept {

    delete _generator;
    _generator = nullptr;

    for (auto obj : _allObjects) {

        delete obj;
    }

    _allObjects.clear();
}

AIObject *AIPlayer::addAIObject(AIAware *object) {

    auto type = object->isBase() ? M_BASEUNIT : dynamic_cast<GameUnit*>(object)->getUnitType();
    AIObject *newAI = new AIObject(type);
    newAI->assignExecutor(object->getUniqueID());

    _allObjects.push_back(newAI);

    return newAI;
}

void AIPlayer::moveCurrentObjectToPoint(GameUnit *currentObject, xVec2 &destination) {

    //"lock" thread
    action = false;


#ifdef __EMSCRIPTEN__

    #ifdef __EMSCRIPTEN_PTHREADS__
    
    syncToMainLoop([currentObject](void *context, GameLogic *sender){

        currentObject->makeMove();
    });  

    #else
    
    currentObject->makeMove();

    #endif

#else 
            
    syncToMainLoop([currentObject](void *context, GameLogic *sender){

        currentObject->makeMove();
    });   

#endif
   
}

void AIPlayer::updateAI(std::vector<GameUnit *> & units, std::vector<GameBase *> & bases) {

    std::vector<int> unitsToRemove;

    //BEGIN NEW TURN-----------------------------------------------------
    _playerMoney += 100;

    for (auto base : bases) {

        bool shouldDecrase = false;

        for (auto unit : units) {

            if (unit->getRequestID() == base->getUniqueID()) {

                if (unit->getTeamID() == _playerID) {

                    shouldDecrase = true;
                    unit->_mode = UNIT_INTERFACE;
                    break;
                }
            }
        }

        if (((base->getTeamID() == _playerID) || (shouldDecrase == true)) && (base->_mode != UNIT_LOCKED)) {

            _playerMoney += 100;

            if (base->_turnsToUnlock > 0) {

                base->_turnsToUnlock--;

                if (base->mayUnlock() == true) {

                    switch (base->_baseAction) {

                        case BASE_BUILD : {

                            for (auto unit : units) {

                                if (unit->getUniqueID() == base->getRequestID()) {

                                    unit->_mode = UNIT_INTERFACE;
                                    unit->_requestBaseID = -1;

                                    for (auto currentAI : _allObjects) {

                                        if(currentAI->exectutorId == unit->getUniqueID()){

                                            currentAI->AIMode = UNIT_NONE;
                                            break;
                                        }
                                    }
                                }
                            }

                            base->_requestUnitID = -1;
                            base->_mode = UNIT_INTERFACE;
                        }
                            break;

                        case BASE_REPAIR : {

                            for (auto unit : units) {

                                if (unit->getUniqueID() == base->getRequestID()) {

                                    unit->_mode = UNIT_INTERFACE;
                                    unit->_requestBaseID = -1;
                                    unit->setSize(10);

                                    for (auto currentAI : _allObjects) {

                                        if(currentAI->exectutorId == unit->getUniqueID()){

                                            currentAI->AIMode = UNIT_NONE;
                                            break;
                                        }
                                    }
                                }
                            }

                            base->_requestUnitID = -1;
                            base->_mode = UNIT_INTERFACE;
                        }
                            break;

                        case BASE_CAPTURED : {

                            for (auto unit : units) {

                                if (unit->getUniqueID() == base->getRequestID()) {

                                    unit->_mode = UNIT_INTERFACE;
                                    unit->_requestBaseID = -1;

                                    base->setTeam(unit->getTeamID());

                                    unitsToRemove.push_back(unit->getUniqueID());
                                    this->removeAI(unit->getUniqueID());
                                }
                            }

                            base->_requestUnitID = -1;
                            base->_mode = UNIT_INTERFACE;

                            this->addAIObject(base);
                        }
                            break;

                        default:
                            break;
                    }

                    for (auto currentAI : _allObjects) {

                        if (currentAI->exectutorId == base->getUniqueID()) {

                            currentAI->AIMode = UNIT_NONE;
                            break;
                        }
                    }
                }
                else {

                    base->_mode = UNIT_INTERFACE;
                }
            }
            else {

                base->_mode = UNIT_INTERFACE;
            }
        }
    }

    syncToMainLoop([unitsToRemove](void *context, GameLogic *sender){
    
        std::vector<int> unitsToRemoveCopy(unitsToRemove);
        
        auto units = sender->getUnits();
        
        for (auto it = units->begin(); it != units->end();) {
            
            if (unitsToRemoveCopy.empty()) {
                break;
            }
            
            auto rit = find(unitsToRemoveCopy.begin(), unitsToRemoveCopy.end(), (*it)->getUniqueID());
            
            if (rit == unitsToRemoveCopy.end()) {
                ++it;
                continue;
            }
            
            unitsToRemoveCopy.erase(rit);
            
            delete *it;
            *it = nullptr;
            
            it = units->erase(it);
        }
        
    });
    
    //create prop table and nil it
    int UNIT_COUNT_TAB[MAXUSER_ALLOWED_TOPLAY];
    memset(UNIT_COUNT_TAB, 0, MAXUSER_ALLOWED_TOPLAY);

    int berserkCount = 0;
    int nullCount = 0;

    //Calculate all units counts for all players
    for (auto unit : units) {

        auto rit = find(unitsToRemove.begin(), unitsToRemove.end(), unit->getUniqueID());
        
        if (rit != unitsToRemove.end()) {
            continue;
        }
        
        UNIT_COUNT_TAB[unit->getTeamID() - 1]++;
    }

    float ratio = 0.0f;
    float d = 0.0;

    //get units ratio
    for (int b = 0; b < MAXUSER_ALLOWED_TOPLAY; ++b) {

        if (b == (_playerID-1)) { continue; }

        d = float(UNIT_COUNT_TAB[b]);
        if (d == 0) { nullCount++; }

        ratio = float(UNIT_COUNT_TAB[(_playerID-1)]) / (d == 0 ? 1 : d);

        if (ratio > 1.5) { berserkCount++; }
    }

    if ((berserkCount == (MAXUSER_ALLOWED_TOPLAY-1)) && (nullCount != (MAXUSER_ALLOWED_TOPLAY-1))) {

        _generator->berserk = true;
    }
    else {

        _generator->berserk = true;
    }

    //-------------------------------------------------------------------

    for (auto it = _allObjects.begin(); it != _allObjects.end();) {

        bool remove = true;

        for (auto unit : units) {

            if (unit->getUniqueID() == (*it)->exectutorId) {
                remove = false;
                break;
            }
        }

        for (auto base : bases) {

            if (base->getUniqueID() == (*it)->exectutorId) {
                remove = false;
                break;
            }
        }

        if (remove) {

            (*it)->AIMode = UNIT_INTERFACE;
        }

        if ((*it)->AIMode == UNIT_INTERFACE) {

            delete *it;
            *it = nullptr;

            it = _allObjects.erase(it);
        }
        else {

            ++it;
        }
    }

    for (auto object : _allObjects) {

        _generator->generatePossibleTasks(object);
    }

    _generator->calculateScore(units, bases);
    _generator->sortTasks();

    auto comparator = [](const AIObject * a, const AIObject * b) -> bool {

        return (a->ownType < b->ownType);
    };

    std::sort(_allObjects.begin(), _allObjects.end(), comparator);

	for (int i = 0; i < _allObjects.size(); ++i) {
		
		auto obj = _allObjects[i];

		if (obj->ownType == M_INFANTRY || obj->ownType == M_BAZOOKA) {
		
			_allObjects.push_back(obj);
			_allObjects.erase(_allObjects.begin() + i);
		}
	}
}

void AIPlayer::executeAI(std::vector<GameUnit *> & units, std::vector<GameBase *> & bases, GameMap * map) {

    std::vector<int> unitsToRemove;

    AITask *currentTask = nullptr;
    bool shouldContinue = false;

    for (int i = 0; i < _allObjects.size(); ++i) { //loop all objects

        auto actual = _allObjects[i];

        if (actual == nullptr) {

            std::cout   << "[WARNING] executeAI AIObject is not available! idx: "
                        << distance(_allObjects.begin(), find(_allObjects.begin(), _allObjects.end(), actual))
                        << std::endl;

            continue;
        }

        for (int c = 0; c < 2; ++c) { //move by whole unit task

#ifdef __EMSCRIPTEN__

    #ifdef __EMSCRIPTEN_PTHREADS__
    
            while (false == action.load()) {

                std::this_thread::yield();
            }

    #endif

#else 

            while (false == action.load()) {

                std::this_thread::yield();
            }

#endif

            if (actual->firstPriorityTask == nullptr) {

                continue;
            }

            if (actual->secondPriorityTask == nullptr) {

                continue;
            }

            switch (c) {
                case 0: {

                    currentTask = actual->firstPriorityTask;
                }
                    break;
                case 1: {

                    currentTask = actual->secondPriorityTask;
                }
                    break;

                default:
                    break;
            }

            if ((currentTask == nullptr) || shouldContinue) {

                shouldContinue = false;
                continue;
            }

            //if end turn do not perform any action for AI if INTERFACE means it will be REMOVED at the end of TURN;
            if((actual->AIMode == UNIT_ENDTURN) ||
               (actual->AIMode == UNIT_INTERFACE) ||
               (actual->AIMode == UNIT_LOCKED)) {

                continue;
            }

            shouldContinue = executeTask(units, bases, map, actual, currentTask, unitsToRemove);

        } //for C < 2

    } // FOR I TO ALLOBJECTS

    _generator->clearTasks();

    for (auto obj : _allObjects) {

        if (obj == nullptr) {
            continue;
        }

        obj->firstPriorityTask = nullptr;
        obj->secondPriorityTask = nullptr;
    }

    //remove destroyed or captured the base

    syncToMainLoop([unitsToRemove](void *context, GameLogic *sender){

        std::vector<int> unitsToRemoveCopy(unitsToRemove);

        auto units = sender->getUnits();

        for (auto it = units->begin(); it != units->end();) {

            if (unitsToRemoveCopy.empty()) {
                break;
            }

            auto rit = find(unitsToRemoveCopy.begin(), unitsToRemoveCopy.end(), (*it)->getUniqueID());

            if (rit == unitsToRemoveCopy.end()) {
                ++it;
                continue;
            }

            unitsToRemoveCopy.erase(rit);

            delete *it;
            *it = nullptr;

            it = units->erase(it);
        }

    });

    for (auto it = _allObjects.begin(); it != _allObjects.end();) {

        if ((*it)->AIMode == UNIT_INTERFACE) {

            delete *it;
            *it = nullptr;

            it = _allObjects.erase(it);
        }
        else {

            ++it;
        }
    }
}

bool AIPlayer::buildUnitTask(std::vector<GameUnit *> & units,
                             std::vector<GameBase *> & bases,
                             GameMap * map,
                             AIObject *actual,
                             AITask *currentTask,
                             std::vector<int> &unitsToRemove) {

    bool shouldContinue = false;

    GameBase *currentBase = nullptr;

    for (auto base : bases) {

        if(actual->exectutorId == base->getUniqueID()){

            currentBase = base;
            break;
        }
    }

    if (currentBase == nullptr) {

        actual->AIMode = UNIT_INTERFACE;
        return shouldContinue;
    }

    if (currentBase->getTeamID() != _playerID) {

        actual->AIMode = UNIT_INTERFACE;
        return shouldContinue;
    }

    if ((actual->AIMode != UNIT_LOCKED) && (currentBase->getUnitMode() != UNIT_LOCKED)) {

        bool occupated = false;

        xVec2 bPos = currentBase->getPosition();
        xVec2 uPos;

        for (auto ou : units) {

            uPos = ou->getRealPosition();

            if (AlmostEqual(bPos, uPos)) {
                occupated = true;
                break;
            }
        }

        if (occupated == false) {

            //Calculate whats going on
            int Costs[5] = {75, 150, 200, 300, 200};

            //Check if there are some bases we can capture
            int infantryCount = 0;
            int baseCount = 0;
            bool buildInfantry = false;

            for (auto tu : units) {

                if ((tu->getTeamID() == _playerID) &&
                    (tu->getUnitType() == M_INFANTRY || tu->getUnitType() == M_BAZOOKA)) {

                    infantryCount++;
                }
            }

            for (auto base : bases) {

                if(base->getUniqueID() == currentBase->getUniqueID()) { continue; }

                if (base->getTeamID() == 0) {

                    baseCount++;
                }
            }

            if (infantryCount < baseCount) {
                buildInfantry = true;
            }

            bool canBuild = true;

            static unsigned int fake = 0;

            srand(unsigned(time(nullptr) + fake));

            fake++;
            if(fake == UINT32_MAX) fake = 0;

            if (buildInfantry) {

                currentBase->_unitToBuild = rand() % 2;

                if (!_hardAI) {

                    if (_playerMoney - Costs[currentBase->_unitToBuild] < 0) {

                        currentBase->_unitToBuild = 0;

                        if (_playerMoney - Costs[currentBase->_unitToBuild] < 0) {

                            canBuild = false;
                        }
                    }
                }

                if (canBuild) {

                    if (!_hardAI) {

                        _playerMoney -= Costs[currentBase->_unitToBuild];
                    }

                    syncToMainLoop([currentBase, this](void *context, GameLogic *sender){

                        auto toCreate = sender->buildUnit(currentBase);
                        auto newObj = this->addAIObject(toCreate);
                        newObj->AIMode = UNIT_LOCKED;
                    });

                    actual->AIMode = UNIT_LOCKED;
                }
            }
            else{

                if (_hardAI) {

                    currentBase->_unitToBuild = rand() % 5;
                }
                else {

                    if (rand() % 2) { //cheap part

                        currentBase->_unitToBuild = (rand() % 3);
                    }
                    else{

                        currentBase->_unitToBuild = 3 + (rand() % 2);
                    }
                }

                if (!_hardAI) {

                    if (_playerMoney - Costs[currentBase->_unitToBuild] < 0) {

                        currentBase->_unitToBuild = rand() % 2;

                        if (_playerMoney - Costs[currentBase->_unitToBuild] < 0) {

                            currentBase->_unitToBuild = 0;
                        }

                        if (_playerMoney - Costs[currentBase->_unitToBuild] < 0)  {

                            canBuild = false;
                        }
                    }
                }

                if (canBuild) {

                    if (!_hardAI) {

                        _playerMoney -= Costs[currentBase->_unitToBuild];
                    }

                    syncToMainLoop([currentBase, this](void *context, GameLogic *sender){

                        auto toCreate = sender->buildUnit(currentBase);
                        auto newObj = this->addAIObject(toCreate);
                        newObj->AIMode = UNIT_LOCKED;
                    });

                    actual->AIMode = UNIT_LOCKED;
                }
            }
        }
    }

    return shouldContinue;
}

bool AIPlayer::captureBaseTask(std::vector<GameUnit *> & units,
                               std::vector<GameBase *> & bases,
                               GameMap * map,
                               AIObject *actual,
                               AITask *currentTask,
                               std::vector<int> &unitsToRemove) {

    bool shouldContinue = false;

    GameUnit *currentObject = nullptr;

    //find unit
    for (auto u : units) {

        if (actual->exectutorId == u->getUniqueID()) {

            currentObject = u;
            break;
        }
    }

    if (currentObject == nullptr) {

        actual->AIMode = UNIT_INTERFACE;
        return shouldContinue;
    }

    if ((actual->AIMode == UNIT_LOCKED) || (actual->AIMode == UNIT_NOTMOVE)) {

        return shouldContinue;
    }

    //find base
    int maxLen = currentObject->_lenghtMove;

    //firts get all point for movements
    auto fPoints = this->getAIPoints(currentObject, maxLen, units, bases);

    //getBase
    GameBase *currBase = nullptr;

    for (auto base : bases) {

        if (currentTask->objectiveId == base->getUniqueID()) {

            currBase = base;
            break;
        }
    }

    //loop points
    if (currBase == nullptr) {

        shouldContinue = true;

        return shouldContinue;
    }

    xVec2 basePos = currBase->getPosition();

    bool baseFound = false;

    for (auto val : fPoints) {

        if (AlmostEqual(basePos, val)) {

            baseFound = true;
            break;
        }
    }

    if (baseFound) {

        basePos = currBase->getAIPosition();
        xVec2 unitTestPoint = currentObject->getAIPosition();

        if (this->canMove(unitTestPoint, basePos) && currBase->getBaseAction() != BASE_CAPTURED) {
            //a star allow to move this point go there

            moveCurrentObjectToPoint(currentObject, currBase->getPosition());
            actual->AIMode = UNIT_LOCKED;

            currBase->_turnsToUnlock = 2;
            currBase->_baseAction = BASE_CAPTURED;
            currBase->_mode = UNIT_LOCKED;
            currentObject->_mode = UNIT_INTERFACE;
            currentObject->_requestBaseID = currBase->getUniqueID();
            currBase->_requestUnitID = currentObject->getUniqueID();

            shouldContinue = true;
        }
        else {

            baseFound = false;
        }
    }

    //if not find closest point to go
    if (!baseFound) {

        xVec2 dir = (currBase->getPosition() - currentObject->getRealPosition());
        dir.normalize();
        xVec2 goToPoint;

        float distMag = (currBase->getPosition() - currentObject->getRealPosition()).mag();
        float targetMag = 0;

        xVec2 A,B;
        std::vector<xVec2> fnPoints;

        for (int it = maxLen; it >= 1; it--) {

            fnPoints = this->getAIPoints(currentObject, it, units, bases);
            goToPoint = this->farthesPositionInDirection(dir, fnPoints);

            targetMag = (goToPoint - currentObject->getRealPosition()).mag();

            A = map->selectionForPosition(currentObject->getRealPosition());
            B = map->selectionForPosition(goToPoint);

            if (this->canMove(A, B) && (targetMag <= distMag)) {
                //a star allow to move this point go there

                moveCurrentObjectToPoint(currentObject, goToPoint);
                actual->AIMode = UNIT_ENDTURN;

                //Check why we were not able to reach point
                auto fUnits = this->getAIEnemyUnitsPos(currentObject, currentObject->_lenghtAttack, units);

                GameUnit *fightUnit = nullptr;

                for (auto u : units) {

                    xVec2 apos = u->getRealPosition();

                    if (this->containsPoint(apos, fUnits) && (u->getTeamID() != currentObject->getTeamID())) {

                        //test if we can attack directly
                        fightUnit = u;
                        break;
                    }
                }

                //There is enemy on the base...
                if (fightUnit != nullptr) {

                    attackUnit(units, bases, fightUnit, currentObject, unitsToRemove);

                    if (currentObject == nullptr) {

                        actual->AIMode = UNIT_INTERFACE;
                    }
                    else {

                        actual->AIMode = UNIT_ENDTURN;
                    }
                }
                else {

                    shouldContinue = true;
                }

                break;
            }

        } //for MAX LEN

    }

    return shouldContinue;
}

bool AIPlayer::repairUnitTask(std::vector<GameUnit *> & units,
                              std::vector<GameBase *> & bases,
                              GameMap * map,
                              AIObject *actual,
                              AITask *currentTask,
                              std::vector<int> &unitsToRemove) {

    bool shouldContinue = false;

    GameUnit *currentObject = nullptr;

    //find unit
    for (auto u : units) {

        if (actual->exectutorId == u->getUniqueID()){
            currentObject = u;
            break;
        }
    }

    if (currentObject == nullptr) {

        actual->AIMode = UNIT_INTERFACE;
        return shouldContinue;
    }

    int maxLen = currentObject->_lenghtMove;

    //firts get all point for movements
    auto fPoints = this->getAIPoints(currentObject, maxLen, units, bases);

    //getBase
    GameBase *currBase = nullptr;

    for (auto *base : bases) {

        if (currentTask->objectiveId == base->getUniqueID()) {
            currBase = base;
            break;
        }
    }

    //loop points
    xVec2 basePos = currBase->getPosition();
    bool baseFound = false;

    for (auto val : fPoints) {

        if (AlmostEqual(basePos, val)) {

            baseFound = true;
            break;
        }
    }

    //even if we founded base check if we can move on it
    for (auto u : units) {

        auto p = u->getRealPosition();

        if (AlmostEqual(basePos, p)) {

            baseFound = false; //you cannot heal someone is on the base
            break;
        }
    }

    if (baseFound) {

        basePos = currBase->getAIPosition();
        xVec2 unitTestPoint = currentObject->getAIPosition();

        if (this->canMove(unitTestPoint, basePos)) {
            //a star allow to move this point go there

            moveCurrentObjectToPoint(currentObject, currBase->getPosition());
            actual->AIMode = UNIT_LOCKED;

            currBase->_turnsToUnlock = 1;
            currBase->_baseAction = BASE_REPAIR;
            currBase->_mode = UNIT_INTERFACE;
            currentObject->_mode = UNIT_INTERFACE;
            currentObject->_requestBaseID = currBase->getUniqueID();
            currBase->_requestUnitID = currentObject->getUniqueID();

            shouldContinue = true;
        }
        else {

            baseFound = false;
        }
    }

    //if not find closest point to go
    if (!baseFound) {

        xVec2 dir = (currBase->getPosition() - currentObject->getRealPosition());
        dir.normalize();
        xVec2 goToPoint;

        float distMag = (currBase->getPosition() - currentObject->getRealPosition()).mag();
        float targetMag = 0;

        xVec2 A, B;
        std::vector<xVec2> fnPoints;

        for (int it = maxLen; it >= 1; it--) {

            fnPoints = this->getAIPoints(currentObject, it, units, bases);
            goToPoint = this->farthesPositionInDirection(dir, fnPoints);

            targetMag = (goToPoint - currentObject->getRealPosition()).mag();

            A = currentObject->getAIPosition();
            B = map->selectionForPosition(goToPoint);

            if (this->canMove(A, B) && (targetMag <= distMag)) {

                //a star allow to move this point go there
                moveCurrentObjectToPoint(currentObject, goToPoint);
                actual->AIMode = UNIT_ENDTURN;
                break;
            }

        } //for MAX LEN

    } 

    return shouldContinue;
}

void AIPlayer::moveAIUnit(std::vector<GameUnit *> &units,
                          std::vector<GameBase *> &bases,
                          GameUnit *fUnit,
                          GameUnit *currentObject,
                          GameMap *map,
                          AIObject *actual,
                          AITask *currentTask,
                          bool isONBase,
                          bool addUnitPosition,
                          bool useDistance,
                          std::function<void()> action) {

    if (fUnit == nullptr) {
        return;
    }

    if (currentObject->_map == nullptr) {
        std::cout << "[ERROR] This is fatal!" << std::endl;
        return;
    }

    for (int ver=0; ver<7; ver++) {

        xVec2 dir = (((addUnitPosition ? fUnit->getRealPosition() : xVec2(0, 0)) + (xVec2(directionVersor[ver], 0) * 64.0)) - currentObject->getRealPosition());
        dir.normalize();

        xVec2 goToPoint;
        xVec2 A, B;

        float distMag = addUnitPosition ? (fUnit->getRealPosition() - currentObject->getRealPosition()).mag() : currentObject->getRealPosition().mag();
        float targetMag = 0;

        bool canGo = true;

        for (int it = currentObject->_lenghtMove; it >= 1; it--) {

            auto fnPoints = this->getAIPoints(currentObject, it, units, bases);

            goToPoint = this->farthesPositionInDirection(dir, fnPoints);

            targetMag = (currentObject->getRealPosition() - goToPoint).mag();

            A = currentObject->getAIPosition();
            B = map->selectionForPosition(goToPoint);

            canGo = true;

            float finalDistance = (currentTask->score > 0.45 ? (128.0) : (isONBase ? 320.0 : 384.0));

            if (this->canMove(A, B) && (targetMag <= distMag) && (useDistance ? (distMag-targetMag >= finalDistance) : true)) {

                for (auto b : bases) {

                    if (AlmostEqual(b->getPosition(), goToPoint)) {
                        canGo = false;
                        break;
                    }
                }

                if(canGo){

                    moveCurrentObjectToPoint(currentObject, goToPoint);
                    actual->AIMode = UNIT_ENDTURN;

                    if (action) {
                        action();
                    }

                    break;
                }

            } else { //IF CAN MOVE TO POINT

                canGo = false;
            }

        } //FOR ITERATOR

        if(canGo) {

            break;
        }
    }
}

void AIPlayer::moveArtillery(GameUnit *currentObject,
                             GameMap * map,
                             AIObject *actual,
                             std::vector<GameBase *> & bases) {

    if (actual->AIMode == UNIT_ENDTURN) {
        return;
    }

    xVec2 A;
    xVec2 B;
    xVec2 goToPoint;

    bool canGo = true;

    for (int post = 0; post < 9; ++post) {

        A = currentObject->getAIPosition();
        B = xVec2(A.x + artilleryVersor[post], A.y);
        goToPoint =  map->positionForSelection(B);

        canGo = true;

        if (this->canMove(A, B)) {

            for (auto b : bases) {

                if (AlmostEqual(b->getPosition(), goToPoint)) {
                    canGo = false;
                    break;
                }
            }

            if (canGo) {

                moveCurrentObjectToPoint(currentObject, goToPoint);
                actual->AIMode = UNIT_ENDTURN;

                break;
            }
        }
    }
}

void AIPlayer::attackUnit(std::vector<GameUnit *> & units,
                          std::vector<GameBase *> & bases,
                          GameUnit *fUnit,
                          GameUnit *currentObject,
                          std::vector<int> &unitsToRemove) {

    auto result = ::attackUnit(currentObject, fUnit, units, bases);

    switch (result) {
        case DESTROY_FIRST: {

            unitsToRemove.push_back(currentObject->getUniqueID());
            currentObject = nullptr;
        }
            break;
        case DESTROY_SECOND: {

            fUnit->setUnitMode(UNIT_NONE);
            unitsToRemove.push_back(fUnit->getUniqueID());
        }
            break;
        case DESTROY_BOTH: {

            fUnit->setUnitMode(UNIT_NONE);
            unitsToRemove.push_back(currentObject->getUniqueID());
            unitsToRemove.push_back(fUnit->getUniqueID());
            currentObject = nullptr;
        }
            break;
        default:
            break;
    }
}

void AIPlayer::simpleAIMove(std::vector<GameUnit *> & units,
                            std::vector<GameBase *> & bases,
                            GameUnit *fUnit,
                            GameUnit *currentObject,
                            GameMap * map,
                            AIObject *actual) {

    for (int ver = 0; ver < 7; ++ver) {

        xVec2 dir = ((fUnit->getRealPosition() + (xVec2(directionVersor[ver],0) * 64.0)) - currentObject->getRealPosition());
        dir.normalize();
        xVec2 goToPoint;
        xVec2 A,B;

        bool canGo = true;

        for (int it = currentObject->_lenghtMove; it >= 1; it--) {

            auto fnPoints = this->getAIPoints(currentObject, it, units, bases);
            goToPoint = this->farthesPositionInDirection(dir, fnPoints);

            A = currentObject->getAIPosition();
            B = map->selectionForPosition(goToPoint);

            canGo = true;

            if (this->canMove(A, B)) {

                for (auto b : bases) {

                    if (AlmostEqual(b->getPosition(), goToPoint)) {

                        canGo = false;
                        break;
                    }
                }

                if (canGo) {

                    moveCurrentObjectToPoint(currentObject, goToPoint);
                    actual->AIMode = UNIT_ENDTURN;

                    break;
                }

            } else {

                canGo = false;
            }

        }

        if (canGo) { break; }
    }
}

void AIPlayer::findNewUnit(std::vector<GameUnit *> & units,
                           std::vector<GameBase *> & bases,
                           GameUnit *fUnit,
                           GameUnit *currentObject,
                           AITask *currentTask,
                           std::vector<xVec2> &funits,
                           std::vector<int> &unitsToRemove,
                           std::function<void()> attackAction,
                           std::function<void()> moveAction,
                           std::function<void()> notFoundAction) {

    vector<GameUnit *> findUnits;

    for (auto u : units) {

        if (u->getTeamID() != currentObject->getTeamID()) {

            findUnits.push_back(u);
        }
    }

    float currLenght = 0;
    float lenght = 1000000;
    int nearID = -1;

    xVec2 aPos = currentObject->getRealPosition();
    xVec2 bPos = xVec2(0,0);

    for (int p = 0; p < findUnits.size(); ++p) {

        auto ab = findUnits[p];
        bPos = ab->getRealPosition();
        currLenght = (aPos-bPos).mag();

        if (currLenght < lenght) {

            nearID = p;
            lenght = currLenght;
        }
    }

    //Ok we get it
    if (nearID != -1) {

        fUnit = findUnits[nearID];
        currentTask->objectiveId = fUnit->getUniqueID();

        xVec2 apos = fUnit->getRealPosition();

        //test if we can attack directly
        if (this->containsPoint(apos, funits)) {
            //yes we can

            attackUnit(units, bases, fUnit, currentObject, unitsToRemove);

            if (attackAction) {

                attackAction();
            }
        }
        else {

            if (moveAction) {

                moveAction();
            }
        }

        return;
    }

    if (notFoundAction) {

        notFoundAction();
    }
}

bool AIPlayer::attackUnitTask(std::vector<GameUnit *> & units,
                              std::vector<GameBase *> & bases,
                              GameMap * map,
                              AIObject *actual,
                              AITask *currentTask,
                              std::vector<int> &unitsToRemove) {

    bool shouldContinue = false;

    GameUnit *currentObject = nullptr;

    //find unit
    for (auto unit : units) {

        if (actual->exectutorId == unit->getUniqueID()) {

            currentObject = unit;
            break;
        }
    }

    if (currentObject == nullptr) {

        actual->AIMode = UNIT_INTERFACE;
        return shouldContinue;
    }

    if (currentObject->getUnitType() == M_ARTILLERY) { //if unit is an alltilery be a bit smarter and try hit enemy unit

        auto funits = this->getAIEnemyUnitsPos(currentObject, currentObject->_lenghtAttack, units);

        GameUnit *fUnit = nullptr;

        for (auto u : units) {

            if (currentTask->objectiveId == u->getUniqueID()) {
                fUnit = u;
                break;
            }
        }

        bool isONBase = false;

        xVec2 tPosA = currentObject->getRealPosition();
        xVec2 tPosB;

        for (auto ba : bases) {

            tPosB = ba->getPosition();

            if (AlmostEqual(tPosA, tPosB)) {
                isONBase = true;
                break;
            }
        }

        if (fUnit != nullptr) {

            //Get enemy unit pos
            xVec2 apos = fUnit->getRealPosition();

            //test if we can attack directly
            if (this->containsPoint(apos, funits) && (!isONBase)) {

                attackUnit(units, bases, fUnit, currentObject, unitsToRemove);

                if (currentObject == nullptr) { //we died...

                    actual->AIMode = UNIT_INTERFACE;

                }
                else { //end turn we are done

                    actual->AIMode = UNIT_ENDTURN;
                }
            }
            else {

                moveAIUnit(units, bases, fUnit, currentObject, map, actual, currentTask, isONBase, true);
                moveArtillery(currentObject, map, actual, bases);
            }
        }
        else { //unit we tryied to attack its destroyed find other one!

            findNewUnit(units, bases, fUnit, currentObject, currentTask, funits, unitsToRemove, [&](){

                if(currentObject == nullptr){ //we died...

                    actual->AIMode = UNIT_INTERFACE;
                }
                else { //end turn we are done

                    actual->AIMode = UNIT_ENDTURN;
                }

            }, [&](){

                moveAIUnit(units, bases, fUnit, currentObject, map, actual, currentTask, isONBase, true);

            }, [&](){

                moveAIUnit(units, bases, fUnit, currentObject, map, actual, currentTask, isONBase, false);
                moveArtillery(currentObject, map, actual, bases);
            });
        }
    }
    else {

        //Test if unit is in attack range
        auto fmUnits = this->getAIEnemyUnitsPos(currentObject, currentObject->_lenghtAttack, units);

        GameUnit *fUnit = nullptr;

        for (auto u : units) {

            if (currentTask->objectiveId == u->getUniqueID()) {
                fUnit = u;
                break;
            }
        }

        if (fUnit != nullptr) {

            //Get enemy unit pos
            xVec2 apos = fUnit->getRealPosition();

            //test if we can attack directly
            if (this->containsPoint(apos, fmUnits)){

                //yes we can
                attackUnit(units, bases, fUnit, currentObject, unitsToRemove);

                if (currentObject == nullptr) { //we died...

                    actual->AIMode = UNIT_INTERFACE;

                } else { //nope we are still here try run out :)

                    if (actual->AIMode == UNIT_NOTMOVE) {

                        actual->AIMode = UNIT_ENDTURN;
                    }
                    else {

                        fUnit = nullptr;

                        for (auto u : units) {

                            if (currentTask->objectiveId == u->getUniqueID()){
                                fUnit = u;
                                break;
                            }
                        }

                        if (fUnit != nullptr) { //yes you should

                            simpleAIMove(units, bases, fUnit, currentObject, map, actual);
                        }
                    }
                }
            }
            else {

                moveAIUnit(units, bases, fUnit, currentObject, map, actual, currentTask, false, true, false, [&](){

                    //Get enemy unit pos
                    xVec2 apos = fUnit->getRealPosition();

                    //test if we can attack directly
                    if (this->containsPoint(apos, fmUnits)) {
                        //yes we can

                        attackUnit(units, bases, fUnit, currentObject, unitsToRemove);

                        if (currentObject == nullptr) { //we died...

                            actual->AIMode = UNIT_INTERFACE;
                        }
                        else { //not died end turn for this unit

                            actual->AIMode = UNIT_ENDTURN;
                        }
                    }

                });
            }
        }
        else { //unit we tryied to attack its destroyed find other one!

            findNewUnit(units, bases, fUnit, currentObject, currentTask, fmUnits, unitsToRemove, [&](){

                if (currentObject == nullptr) { //we died...

                    actual->AIMode = UNIT_INTERFACE;
                }
                else { //nope we are still here try run out :)

                    if (actual->AIMode == UNIT_NOTMOVE) {

                        actual->AIMode = UNIT_ENDTURN;
                    }
                    else {

                        //First test if you need to ... :]
                        fUnit = nullptr;

                        for (auto u : units) {

                            if (actual->exectutorId == u->getUniqueID()) {

                                fUnit = u;
                                break;
                            }
                        }

                        if ((fUnit != nullptr) && ( rand() % 2 )) { //yes you should

                            simpleAIMove(units, bases, fUnit, currentObject, map, actual);
                        }
                    }
                }

            }, [&](){

                moveAIUnit(units, bases, fUnit, currentObject, map, actual, currentTask, false, true, false, [&](){

                    //Get enemy unit pos
                    xVec2 apos = fUnit->getRealPosition();

                    //test if we can attack directly
                    if (this->containsPoint(apos, fmUnits)) {
                        //yes we can

                        attackUnit(units, bases, fUnit, currentObject, unitsToRemove);

                        if (currentObject == nullptr) { //we died...

                            actual->AIMode = UNIT_INTERFACE;
                        }
                        else { //not died end turn for this unit

                            actual->AIMode = UNIT_ENDTURN;
                        }
                    }
                });

            });
        }
    }

    if (actual->AIMode != UNIT_INTERFACE) {

        actual->AIMode = UNIT_ENDTURN;
    }

    return shouldContinue;
}

bool AIPlayer::executeTask(std::vector<GameUnit *> & units,
                           std::vector<GameBase *> & bases,
                           GameMap * map,
                           AIObject *actual,
                           AITask *currentTask,
                           std::vector<int> &unitsToRemove) {

    bool shouldContinue = false;

    switch (currentTask->Priority) {

        case AI_BUILD_UNIT: {

            shouldContinue = buildUnitTask(units, bases, map, actual, currentTask, unitsToRemove);
        }
            break;

        case AI_DEFENSE_BASES:
        case AI_ATTACK_ENEMY: {

            shouldContinue = attackUnitTask(units, bases, map, actual, currentTask, unitsToRemove);
        }
            break;

        case AI_REPAIR_UNIT: {

            shouldContinue = repairUnitTask(units, bases, map, actual, currentTask, unitsToRemove);
        }
            break;

        case AI_CAPTURE_BASES: {

            shouldContinue = captureBaseTask(units, bases, map, actual, currentTask, unitsToRemove);
        }
            break;

        case AI_STAY_UNIT:
        case AI_DO_NOTHING_PRIOR:
            break;

        default:
            break;
    }

    return shouldContinue;
}

void AIPlayer::removeAI(const int & unitID) {

    for (int i = 0; i < _allObjects.size(); ++i) {

        auto it = _allObjects[i];

        if (it->exectutorId == unitID) {

            it->AIMode = UNIT_INTERFACE;
            break;
        }
    }
}

xVec2 AIPlayer::farthesPositionInDirection(const xVec2 & direction, const std::vector<xVec2> & points) {

    int fId = -1;

    if (points.size() == 0) {

        return xVec2(0, 0);
    }

    auto p = points[0];

    float fd = direction.dot(p);
    float tmp;

    for (int i = 0; i < points.size(); ++i) {

        p = points[i];

        if (p.x == 0 && p.y == 0) { continue; }

        tmp = direction.dot(p);

        if (tmp > fd) {

            fd = tmp;
            fId = i;
        }
    }

    if (fId == -1) {

        return xVec2(0, 0);
    }

    return points[fId];
}

bool AIPlayer::containsPoint(const xVec2 & p, const std::vector<xVec2> & points) {

    bool contains = false;
    
    for (auto c : points) {
    
        if (AlmostEqual(p, c)) {
            contains = true;
            break;
        }
    }
    
    return contains;
}

std::vector<xVec2> AIPlayer::getAIPoints(GameUnit* current, const int & maxlenght, std::vector<GameUnit *> & units, std::vector<GameBase *> & bases) {

    std::vector<xVec2> PointsToReturn;
    
    if (current == nullptr) return PointsToReturn;
    
    //Step 2 set Road for all hex that are not locked and not occuped by any other unit
    
    xVec2 SelectPoint   = _map->selectionForPosition(current->getRealPosition());
    TTile **CurrMap     = _map->getMap();
    xVec2 MapMAX        = _map->getMapSize();
    
    int STEP = maxlenght;
    int a =0, b=0;
    
    //Fill first line not need to specify if its even or not
    a = SelectPoint.x - STEP * 2;
    b = SelectPoint.y;
    
    int ax = 0;
    int ay = 0;
    xVec2 TestPoint;
    xVec2 TestPoint2;
    bool nBreak = false;
    
    for(int f=0; f<STEP*2+1; f++){
        
        ax = a + (f*2);
        ay = b;
        
        if ((ax < 0) || (ax >= MapMAX.x)) continue; //out of map?
        TestPoint = _map->positionForSelection(xVec2(ax, ay));
        
        if (!FLT_EQUAL(TestPoint.x, 0) && !FLT_EQUAL(TestPoint.y, 0)) { //if this point its locked we not need to test it
        
            //but if its not locked test if there is a unit on it so we can skip it
            for (auto A : units){
                
                TestPoint2 = A->getRealPosition();

                if (AlmostEqual(TestPoint, TestPoint2) && A->getUnitMode() != UNIT_NONE) {
                   
                    nBreak = true;
                    break;
                }
            }
            
            if ((current->getUnitType() != M_INFANTRY) && (current->getUnitType() != M_BAZOOKA)) {
                
                for (auto Base : bases) {
                    
                    TestPoint2 = Base->getPosition();
                    
                    if (AlmostEqual(TestPoint, TestPoint2) && (Base->getTeamID() != current->getTeamID())) {
                        
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
        
        CurrMap[ax][ay].isRoad = true;
        PointsToReturn.push_back(TestPoint);
    }
    
    int Versor = (((int)SelectPoint.x % 2) ? 1 : -1);
    int d = 0;
    int e = 0;
    int g = 0;
    
    for (int c = 1; c < STEP+1; c++) { //up and down so much time as step count
        
        d++;
        nBreak = false;
        
        if (!(d%2)) e++;
        
        for (int f = 0; f < STEP*2+1-c; f++) {
            
            ax = a + (f*2) + c;
            ay = b = SelectPoint.y-e*Versor;
            
            if ((ax < 0)||(ax >= MapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= MapMAX.y)) continue;
            
            TestPoint = _map->positionForSelection(xVec2(ax, ay));
            
            if (!FLT_EQUAL(TestPoint.x, 0) && !FLT_EQUAL(TestPoint.y, 0)) { //if this point its locked we not need to test it
            
                //but if its not locked test if there is a unit on it so we can skip it
                for (auto A : units) {
                    
                    TestPoint2 = A->getRealPosition();

                    if (AlmostEqual(TestPoint, TestPoint2) && A->getUnitMode() != UNIT_NONE) {
                     
                        nBreak = true;
                        break;
                    }
                }
                
                if ((current->getUnitType() != M_INFANTRY) && (current->getUnitType() != M_BAZOOKA)) {
                    
                    for (auto Base : bases) {
                       
                        TestPoint2 = Base->getPosition();
                        
                        if (AlmostEqual(TestPoint, TestPoint2) && (Base->getTeamID() != current->getTeamID())) {
                            
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
            
            CurrMap[ax][ay].isRoad = true;
            PointsToReturn.push_back(TestPoint);
        }
        
        nBreak = false;
        
        if ((d%2)) g++;
        
        //bottom test
        for ( int f = 0; f < STEP*2+1-c; f++) {
            
            ax = a + (f*2) + c;
            ay = b = SelectPoint.y+g*Versor;
            
            if ((ax < 0)||(ax >= MapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= MapMAX.y)) continue;
            
            TestPoint = _map->positionForSelection(xVec2(ax, ay));
            
            if (!FLT_EQUAL(TestPoint.x, 0) && !FLT_EQUAL(TestPoint.y, 0)) { //if this point its locked we not need to test it
            
                //but if its not locked test if there is a unit on it so we can skip it
                for (auto A : units) {
                   
                    TestPoint2 = A->getRealPosition();

                    if (AlmostEqual(TestPoint, TestPoint2) && A->getUnitMode() != UNIT_NONE) {
                        
                        nBreak = true;
                        break;
                    }
                }
                
                if ((current->getUnitType() != M_INFANTRY) && (current->getUnitType() != M_BAZOOKA)) {
                    
                    for (auto Base : bases) {
                        
                        TestPoint2 = Base->getPosition();
                        
                        if (AlmostEqual(TestPoint, TestPoint2) && (Base->getTeamID() != current->getTeamID())) {
                            
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
            
            CurrMap[ax][ay].isRoad = true;
            PointsToReturn.push_back(TestPoint);
        }
    }
    
    _map->lockRoad();
    
    return PointsToReturn;
}

bool AIPlayer::canMove(const xVec2 & start, const xVec2 & end) {

    if (_map->canMoveFromTo(start, end)) {
        
        _map->cleanRoad();

        return true;
    }
    
    _map->cleanRoad();
    return false;
}

std::vector<xVec2> AIPlayer::getAIEnemyUnitsPos(GameUnit* current, const int & maxlenght, std::vector<GameUnit *> & units) {

    std::vector<xVec2> unitsPoints;
    auto map = current->_map;
    
    if (current == nullptr || map == nullptr) return unitsPoints;
    
    xVec2 SelectPoint = map->selectionForPosition(current->getRealPosition());
    xVec2 MapMAX      = map->getMapSize();
    
    int STEP = maxlenght;
    int a =0, b=0;
    
    a = SelectPoint.x - STEP * 2;
    b = SelectPoint.y;
    
    int ax = 0;
    int ay = 0;
    xVec2 TestPoint;
    xVec2 TestPoint2;
    
    for (int f = 0; f < STEP*2+1; ++f) {
        
        ax = a + (f*2);
        ay = b;
        
        if ((ax < 0)||(ax >= MapMAX.x)) continue;
        
        TestPoint =  map->positionForSelection(xVec2(ax, ay));
        
        if (!FLT_EQUAL(TestPoint.x, 0) && !FLT_EQUAL(TestPoint.y, 0)) { //if this point its locked we not need to test it
        
            //but if its not locked test if there is a unit on it so we can skip it
            for (auto A : units) {
                
                TestPoint2 = A->getRealPosition();
                
                if (AlmostEqual(TestPoint, TestPoint2)) {
                    
                    if (A->getTeamID() != current->getTeamID()) {
                        
                        unitsPoints.push_back(TestPoint);
                    }
                }
            }
        }
        
    }
    
    int Versor = (((int)SelectPoint.x % 2) ? 1 : -1);
    int d = 0;
    int e = 0;
    int g = 0;
    
    for (int c = 1; c < STEP+1; c++) { //up and down so much time as step count
        
        d++;
        
        if (!(d%2)) e++;
        
        for (int f = 0; f < STEP*2+1-c; f++) {
            
            ax = a + (f*2) + c;
            ay = b = SelectPoint.y-e*Versor;
            
            if ((ax < 0)||(ax >= MapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= MapMAX.y)) continue;
            
            TestPoint = _map->positionForSelection(xVec2(ax, ay));
            
            if (!FLT_EQUAL(TestPoint.x, 0) && !FLT_EQUAL(TestPoint.y, 0)) { //if this point its locked we not need to test it
            
                //but if its not locked test if there is a unit on it so we can skip it
                for (auto A : units) {
                    
                    TestPoint2 = A->getRealPosition();
                    
                    if (AlmostEqual(TestPoint, TestPoint2)) {
                        
                        if (A->getTeamID() != current->getTeamID()) {
                            
                            unitsPoints.push_back(TestPoint);
                        }
                    }
                }
            }
        }
        
        if ((d%2)) g++;
        
        //bottom test
        for (int f = 0; f < STEP*2+1-c; f++) {
            
            ax = a + (f*2) + c;
            ay = b = SelectPoint.y+g*Versor;
            
            if ((ax < 0)||(ax >= MapMAX.x)) continue; //out of map?
            if ((ay < 0)||(ay >= MapMAX.y)) continue;
            
            TestPoint = _map->positionForSelection(xVec2(ax, ay));
            
            if (!FLT_EQUAL(TestPoint.x, 0) && !FLT_EQUAL(TestPoint.y, 0)) { //if this point its locked we not need to test it
            
                //but if its not locked test if there is a unit on it so we can skip it
                for (auto A : units) {
                    
                    TestPoint2 = A->getRealPosition();
                    
                    if (AlmostEqual(TestPoint, TestPoint2)) {
                        
                        if (A->getTeamID() != current->getTeamID()) {
                            
                            unitsPoints.push_back(TestPoint);
                        }
                    }
                }
            }
        }
    }
    
    return unitsPoints;
}
