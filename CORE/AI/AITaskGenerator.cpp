//
//  AITaskGenerator.cpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "AITaskGenerator.hpp"
#include "GameUnit.hpp"
#include "GameBase.hpp"

void AITaskGenerator::generatePossibleTasks(AIObject *current) {

    if (current->AIMode != UNIT_LOCKED) {

        current->AIMode = UNIT_NONE;
    }

    AIAction *action = new AIAction();

    action->owner = current;

    for (int i = 0; i<current->prorities.size(); ++i) {

        AITask *task = new AITask();

        task->Priority = (AI_ACTIONS_PRIORITY)current->prorities[i];

        action->taskToAssign.push_back(task);
    }

    _actions.push_back(action);
}

void AITaskGenerator::calculateScore(const std::vector<GameUnit *> & units, const std::vector<GameBase *> & bases) {

    //modifier = [0..10]
    //score = (MAX_PRIORITY - priority + modifier) / distance
    float modifier = 0.0f;
    float ownprority = 0.0f;
    float distance = 100.0f;
    long pCount = 0.0;

    AIAction *currentAction = nullptr;
    AITask *currentTask = nullptr;

    AIAware *currentObject = nullptr;
    bool isBase = false;

    //loop all objects actions
    for (int i =0 ; i < _actions.size(); ++i) {

        currentAction = _actions[i];

        //loop all possible task for this Object
        currentObject = nullptr;

        //FIND OUT OBJECT IN GAME
        for (auto unit : units) {

            if (currentAction->owner->exectutorId == unit->getUniqueID()) {
                currentObject = unit;
                isBase = false;
                break;
            }
        }

        if (currentObject == nullptr) {

            for (auto base : bases) {

                if (currentAction->owner->exectutorId == base->getUniqueID()) {
                    currentObject = base;
                    isBase = true;
                    break;
                }
            }
        }

        if (currentObject == nullptr) {
            std::cerr << "[AI] TRYING TO SCORE OBJECT THAT NOT EXIST: " << currentAction->owner->exectutorId << std::endl;
            continue;
        }

        pCount = currentAction->taskToAssign.size();

        for (int t = 0; t < pCount; ++t) {

            currentTask = currentAction->taskToAssign[t];

            switch (currentTask->Priority) {

                case AI_CAPTURE_BASES : {

                    if (isBase == false) {

                        GameUnit *unit = dynamic_cast<GameUnit*>(currentObject);

                        if (unit == nullptr) {
                            std::cerr << "[AI] Fatal unit is not an GAMEUNIT" << std::endl;
                            continue;
                        }

                        //Look if exist BASES that can be captured
                        auto tocapture = vector<GameBase *>();

                        for (auto base : bases) {

                            if (base->getTeamID() != unit->getTeamID()) {

                                tocapture.push_back(base);
                            }
                        }

                        int nearID = -1;
                        float lenght = FLT_MAX;
                        float currLenght = -1.0;

                        if (tocapture.size() != 0) {

                            //FIND NEAREST BASE

                            xVec2 aPos = unit->getRealPosition();
                            xVec2 bPos = xVec2(0,0);
                            bool skip = false;

                            for (int p = 0; p < tocapture.size(); ++p) {

                                auto ab = tocapture[p];
                                skip = false;

                                for (auto punit : units) {

                                    if (punit == nullptr) {
                                        continue;
                                    }

                                    if (ab->isOccupated(punit)) {

                                        if (punit->getTeamID() == unit->getTeamID()){
                                            skip = true;
                                            break;
                                        }
                                    }
                                }

                                if (skip) {

                                    currLenght = 10000.0;
                                }
                                else {

                                    bPos = ab->getPosition();
                                    currLenght = (aPos-bPos).mag();
                                }

                                if (currLenght < lenght) {
                                    nearID = p;
                                    lenght = currLenght;
                                }
                            }

                            //WE FIND OUR NEAR BASE
                            if (nearID != -1) {

                                auto findBase = tocapture[nearID];

                                //Store near Lenght to base
                                distance = lenght;

                                //own ID
                                currentTask->objectiveId = findBase->getUniqueID();

                                if (findBase->getTeamID() == 0) {

                                    modifier = 10; //empty base! high capture priority
                                }
                                else {

                                    //We are not so exited about this base
                                    modifier = 8;

                                    //see if its quarded maximum 5 Units
                                    int MAX_DECRASE_COUNTER = 5;

                                    xVec2 initialPos = findBase->getPosition();
                                    xVec2 uPos = xVec2(0,0);

                                    for (auto eUnit : units) {

                                        if (eUnit->getTeamID() != unit->getTeamID()) {

                                            uPos = eUnit->getRealPosition();

                                            if ((initialPos - uPos).mag() < 128.0) { //enemy unit close base

                                                MAX_DECRASE_COUNTER--;
                                                if(MAX_DECRASE_COUNTER == 0) break;

                                                modifier--;
                                            }
                                        }
                                    }
                                }
                            }
                            else { //should never happend

                                std::cerr << "[AI] Fatal: did not found near BASE" << std::endl;
                                modifier = 0;
                            }
                        }
                        else {

                            modifier = 0;
                        }
                    }
                    else {

                        modifier = 0;
                    }
                }
                    break;

                case AI_ATTACK_ENEMY : {

                    auto unit = dynamic_cast<GameUnit*>(currentObject);

                    if (unit == nullptr) {
                        std::cerr << "[AI] Fatal unit is not an GAMEUNIT" << std::endl;
                        continue;
                    }

                    GameUnit *fUnit = nullptr;

                    int lifetosearch = 10;
                    int maxID = 0;

                    int udx[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

                    bool done = false;
                    //look for
                    for (int i = 0; i < units.size(); ++i) {

                        fUnit = units[i];

                        if (fUnit->getUniqueID() == unit->getUniqueID()) { continue; }

                        if (fUnit->getTeamID() != unit->getTeamID()) {

                            if ((fUnit->getSize() == lifetosearch) ||
                                (fUnit->getSize() == (lifetosearch-1))) {

                                udx[maxID] = i;
                                maxID++;
                            }

                            if (maxID == 8) done = true;
                        }

                        if (!(maxID % 2)) { lifetosearch -= 2; }

                        if (done) { break; }
                    }

                    int pID = -1;
                    int fID = -1;

                    float currLenght = 0;
                    float lenght = 1000000;
                    xVec2 thisUnit = unit->getRealPosition();
                    xVec2 findUnit = xVec2(0,0);

                    for (int f = 0; f < 8; ++f) {

                        pID = udx[f];

                        if (pID == -1) { continue; }

                        fUnit = units[pID];
                        findUnit = fUnit->getRealPosition();

                        currLenght = (thisUnit - findUnit).mag();

                        if (currLenght <= lenght) {

                            fID = pID;
                            lenght = currLenght;
                        }
                    }

                    modifier = 6;

                    if (fID != -1) {

                        fUnit = units[fID];
                        currentTask->objectiveId = fUnit->getUniqueID();

                        thisUnit = unit->getRealPosition();
                        findUnit = fUnit->getRealPosition();

                        if (fUnit->getSize() < 10) {
                            modifier += 1;
                        }
                        else if(fUnit->getSize() < 8) {
                            modifier += 2;
                        }
                        else if(fUnit->getSize() < 6) {
                            modifier += 3;
                        }
                        else if(fUnit->getSize() < 4) {
                            modifier += 4;
                        }

                        distance = (thisUnit - findUnit).mag();

                        if(distance > 320.0) { modifier -= 1; }
                        if(distance > 448.0) { modifier -= 1; }
                        if(distance > 512.0) { modifier -= 1; }

                        if(modifier <= 0) { modifier = 1; }
                    }
                    else {

                        modifier = 0;
                        distance = 10000.0;
                        std::cout << "[AI] Unable to find any unit to attack" << std::endl;
                    }
                }
                    break;

                case AI_DEFENSE_BASES :{
                    //zalozenie jest takie ze jak juz wrogi unit stoi na twoim polu to mod = 10 inaczej cos wymysl =p

                    std::vector<int> capturedID;
                    auto unit = dynamic_cast<GameUnit*>(currentObject);

                    if (unit == nullptr) {
                        std::cerr << "[AI] Fatal unit is not an GAMEUNIT" << std::endl;
                        continue;
                    }

                    GameBase *base = nullptr;
                    GameUnit *oUnit = nullptr;

                    xVec2 basePos = xVec2(0,0);
                    xVec2 unitPos = xVec2(0,0);

                    for (int b = 0; b < bases.size(); ++b) {

                        base = bases[b];

                        if (unit->getTeamID() == base->getTeamID()) {

                            //our base look if any enemy unit is on it
                            basePos = base->getPosition();

                            for (int e = 0; e < units.size(); ++e) {

                                oUnit = units[e];

                                if (oUnit->getUniqueID() == unit->getUniqueID()) { continue; }

                                //enemy unit
                                if ((oUnit->getTeamID() != unit->getTeamID()) &&
                                   ((oUnit->getUnitType() == M_INFANTRY) || (oUnit->getUnitType() == M_BAZOOKA))) {

                                    unitPos = oUnit->getRealPosition();

                                    if (AlmostEqual(unitPos, basePos)) { //not good someone capturing our base...

                                        capturedID.push_back(e);
                                        
                                    }//if equal

                                }//if teamID

                            }//for UNITS

                        } //if Unit == Base
                        
                    }//for Bases

                    //we get some important pos
                    int nearID = -1;
                    float lenght = FLT_MAX;
                    float currLenght = -1.0;
                    xVec2 aUnitPos = unit->getRealPosition();

                    if (capturedID.empty()) { //try again looking for some near base units

                        for (int b = 0; b < bases.size(); ++b) {

                            base = bases[b];

                            if (unit->getTeamID() == base->getTeamID()) {

                                //our base look if any enemy unit is on it
                                basePos = base->getPosition();

                                for (int e = 0; e < units.size(); ++e) {

                                    oUnit = units[e];

                                    if (oUnit->getUniqueID() == unit->getUniqueID()) { continue; }

                                    //enemy unit
                                    if ((oUnit->getTeamID() != unit->getTeamID()) &&
                                        ((oUnit->getUnitType() == M_INFANTRY) || (oUnit->getUnitType() == M_BAZOOKA))) {

                                        unitPos = oUnit->getRealPosition();

                                        currLenght = (basePos - unitPos).mag();

                                        if (currLenght <= 256.0) { //get near units

                                            capturedID.push_back(e);
                                        }

                                    }//if teamID

                                }//for UNITS

                            } //if Unit == Base

                        }//for Bases

                        if (capturedID.empty()) {

                            modifier = 0;
                            distance = 1000.0;
                        }
                        else{ //some units near base look for closest

                            for (int a = 0; a < capturedID.size(); a++) {

                                oUnit = units[capturedID[a]];
                                unitPos = oUnit->getRealPosition();

                                currLenght = (aUnitPos - unitPos).mag();

                                if(currLenght < lenght){

                                    nearID = capturedID[a];
                                    lenght = currLenght;
                                }
                            }

                            if (nearID != -1) {

                                oUnit = units[nearID];

                                currentTask->objectiveId = oUnit->getUniqueID();

                                distance = lenght;

                                if (distance <= 256.0) {
                                    modifier = 4;
                                }
                                else if(distance <= 64.0 * 3.0) {
                                    modifier = 6;
                                }
                                else if (distance <= 64.0 * 2.0) {
                                    modifier = 8;
                                }
                            }
                            else {

                                modifier = 0;
                                distance = 1000.0;
                            }
                        }
                    }
                    else { //there are some units

                        for (int a = 0; a < capturedID.size(); ++a) { //loop and get near unit

                            oUnit = units[capturedID[a]];
                            unitPos = oUnit->getRealPosition();

                            currLenght = (aUnitPos - unitPos).mag();

                            if (currLenght < lenght) {
                                nearID = capturedID[a];
                                lenght = currLenght;
                            }
                        }

                        if (nearID != -1) {
                            oUnit = units[nearID];

                            currentTask->objectiveId = oUnit->getUniqueID();
                            modifier = 10;

                            distance = lenght;
                        }
                        else {

                            modifier = 0;
                            distance = 1000.0;
                        }
                    }
                }
                    break;

                case AI_REPAIR_UNIT :{

                    if (isBase == false) {

                        auto unit = dynamic_cast<GameUnit*>(currentObject);

                        if (unit == nullptr) {
                            std::cerr << "[AI] Fatal unit is not an GAMEUNIT" << std::endl;
                            continue;
                        }

                        GameBase *tBase = nullptr;

                        //get damage
                        modifier = 10 - unit->getSize();

                        //find nearest base that is able for repair
                        xVec2 aPos = unit->getRealPosition();
                        xVec2 bPos = xVec2(0,0);

                        int nearID = -1;
                        float lenght = FLT_MAX;
                        float currLenght = -1.0;

                        if (modifier > 0) {

                            for (int a = 0; a < bases.size(); ++a) {

                                tBase = bases[a];

                                if (tBase->getTeamID() == unit->getTeamID()) {

                                    bPos = tBase->getPosition();
                                    currLenght = (aPos-bPos).mag();

                                    if (currLenght < lenght) {

                                        nearID = a;
                                        lenght = currLenght;
                                    }
                                }
                            }

                            distance = lenght;

                            if(nearID != -1){

                                tBase = bases[nearID];
                                currentTask->objectiveId = tBase->getUniqueID();
                            }
                            else {

                                distance = FLT_MAX;
                                currentTask->objectiveId = -1;
                            }
                        }
                        else {

                            distance = FLT_MAX;
                        }
                    }

                }
                    break;

                case AI_STAY_UNIT : {

                    modifier = 1;
                    distance = 1000.0;
                }
                    break;

                case AI_BUILD_UNIT : { //try to build all the time
                    
                    modifier = 10;
                    distance = 64.0;
                }

                default:
                    break;
            }

            ownprority = (float)(pCount - t);

            currentTask->score = (MAX_PRIORITY - (float)currentTask->Priority + modifier + ownprority) / distance;

            GameUnit *testCaptureUnit = nullptr;

            if ((currentObject != nullptr) && (isBase == false)) {

                testCaptureUnit = dynamic_cast<GameUnit*>(currentObject);
            }

            if ((currentTask->Priority == AI_ATTACK_ENEMY) && (berserk)) {

                if (testCaptureUnit != nullptr) {

                    if ((testCaptureUnit->getUnitType() != M_INFANTRY) && (testCaptureUnit->getUnitType() != M_BAZOOKA)) {

                        currentTask->score += 0.5;
                    }
                    else {

                        currentTask->score -= 0.1;
                    }
                }
            }

        }//end of TASK TO ASSIGN
    }//end of ACTIONS

    berserk = false;
}

void AITaskGenerator::sortTasks() {

    auto comparator = [](AITask *a, AITask *b) -> bool {

        return (a->score > b->score);
    };

    for (auto action : _actions) {

        sort(action->taskToAssign.begin(), action->taskToAssign.end(),
             comparator);

        this->assignTasksToObject(action->taskToAssign, action->owner);
    }
}

void AITaskGenerator::assignTasksToObject(const std::vector<AITask *> & tasks, AIObject *object) {

    if (tasks.size() >= 2) {

        object->assignTask(tasks[0]);
        object->assignTask(tasks[1]);
        return;
    }

    object->assignTask(tasks[0]);
}

void AITaskGenerator::clearTasks() {

    for (auto a : _actions) {

        delete a;
    }

    _actions.clear();
    std::vector<AIAction *>().swap(_actions);
}
