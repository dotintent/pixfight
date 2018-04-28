//
//  GameMap.hpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <fstream>

#include "Core-pch.hpp"
#include "Drawable.hpp"
#include "FurionGL.hpp"
#include "Mesh2d.hpp"
#include "SearchNode.hpp"
#include "MathTypes.h"

class GameMap final : public Drawable {

    constexpr static const uint32_t MAX_PLAYERS_DEF = 4;
    constexpr static const int32_t HEADER_ID = 0xFFAD;

public:

    GameMap(const std::string & texPathDirectory);
    ~GameMap() noexcept;

    GameMap(const GameMap & other) = delete;
    GameMap(GameMap && other) noexcept = delete;

    GameMap & operator= (const GameMap & other) = delete;
    GameMap & operator= (GameMap && other) noexcept = delete;

    void setup(const int & x, const int & y, const int & Rx, const int & Ry);
    void resize(const int & x, const int & y);
    virtual void draw(const DrawingContext & context) override;

    xVec2 getBias();
    xVec2 getMapSize();
    xVec2 getRenderSize();
    TTile** getMap();
    float getCurrendScale();

    void lockRoad();
    void cleanRoad();
    void cleanAStar();
    bool isPointEqual(const xVec2 & a, const xVec2 & b);

    void move(const float & mx, const float & my, float delta);
    void move(const xVec2 & npos);
    void allowScaling(bool flag);
    virtual void setScale(const float & scale) override;
    void setMinMaxScale(const float & min, const float & max);
    void testPoint(const xVec2 & p);
    void selectTile(const xVec2 & p);
    bool selectEndPoint(const xVec2 & p);
    bool canMoveFromTo(const xVec2 & start, const xVec2 & end);
    xVec2 positionForSelection(const xVec2 & p);
    xVec2 selectionForPosition(const xVec2 & p);
    xVec2 hitTest();
    xVec2 startPosition();
    xVec2 endPosition();

    bool canAttackAtTestPoint();
    bool canMergeAtTestPoint();

    //-----EDIT
    void textureForLayer(const int & i, cFGLT* newTex);
    void setTileForLayer(const int & i, const xVec2 & xy);
    void clearTile(const int & i);
    void fitTextureForLayer(const int & i);

    void lockHex();
    void unlockHex();

    //----SAVE/LOAD
    bool load(const std::string & path);
    bool save(const std::string & path);

    //---UTILS
    void saveCamForPlayer(const int & i);
    void loadCamForPlayer(const int & i);
    void changeRenderOffset(const int & rx, const int & ry);

    std::vector<xVec2> & getAStar() { return AStarWay; }

#ifdef MAP_EDITOR
    int penSize;
    bool hideLock;
#endif

private:

    std::string _rootPath;

    typedef struct TexStruct{
        uint32_t Width;
        uint32_t Height;
        uint32_t TileWidth;
        uint32_t TileHeight;
        uint32_t tiles_x;
        uint32_t tiles_y;
    } TexStruct;

    cFGLT *_tex[MAX_ACTIVE_LAYERS]; //Tiletexture
    cFGLT _roadTex;

    Mesh2d *_layer[MAX_ACTIVE_LAYERS]; //mesh for map drawing
    Mesh2d *_road;

    TTile **_mapTiles;

    int32_t     _maxX, _maxY;
    int32_t     _renderX, _renderY;
    float   _sx, _sy;
    int     _offX, _offY;
    int     _px, _py;
    float   _rx, _ry;
    int     _fx, _fy;

    bool _scalable;

    float _mins, _maxs;

    int32_t _drawOffsetX, _drawOffsetY;

    xVec2 _selectedPoint;
    xVec2 _drawTile;
    xVec2 _endPoint;
    xVec2 _testVec;
    xVec2 _camPos[MAX_PLAYERS_DEF];

    AStarSearch<SearchNode> _astarsearch;
    SearchNode _nodeStart;
    SearchNode _nodeEnd;

    std::vector<xVec2> AStarWay;
};
