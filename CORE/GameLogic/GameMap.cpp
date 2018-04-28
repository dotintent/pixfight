//
//  GameMap.cpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameMap.hpp"
#include "glTextureLoader.hpp"

GameMap::GameMap(const std::string & texPathDirectory) {

    _rootPath = texPathDirectory;

    std::string trackingPath = _rootPath + "tracking.png";

    _roadTex.iTexture = textureLoader.loadFile(trackingPath, GL_LINEAR, 0, GL_REPEAT, true);
    _roadTex.Width  = 256;
    _roadTex.Height =  128;
    _roadTex.TileWidth = 64;
    _roadTex.TileHeight = 64;
    _roadTex.tiles_x = 2;
    _roadTex.tiles_y = 2;
    _roadTex.active = true;

    _maxX = 0;
    _maxY = 0;

    _road = new Mesh2d(&_roadTex);

    _mapTiles = nullptr;

    _renderX = 0;
    _renderY = 0;

    _sx = 0;
    _sy = 0;

    _scale = false;

    _scale = 1.0f;
    _mins = 1.0f;
    _maxs = 1.0f;

    _drawOffsetX = 0;
    _drawOffsetY = 0;

    for (int r=0; r<MAX_ACTIVE_LAYERS; ++r) {
        _layer[r] = nullptr;
        _tex[r] = nullptr;
    }

    _fx = 0;
    _fy = 0;

    _selectedPoint = xVec2(-100,-100);
    _endPoint = xVec2(-100,-100);

    _nodeStart.x = 0;
    _nodeStart.y = 0;

    _testVec  = xVec2(0,0);
    _drawTile = xVec2(0,0);

    for (int i=0; i<MAX_PLAYERS_DEF; i++) {

        _camPos[i] = xVec2(0,0);
    }

#ifdef MAP_EDITOR
    penSize = 0;
    hideLock = false;
#endif
}

GameMap::~GameMap() noexcept {

    _mapwsk = nullptr;
    MAP_WIDTH = 0;
    MAP_HEIGHT = 0;

    for (int i=0; i<_maxX; i++) {

        delete [] _mapTiles[i];
        _mapTiles[i] = nullptr;
    }

    delete [] _mapTiles;
    _mapTiles = NULL;

    if (_road) {

        delete _road;
        _road = nullptr;
    }

    for (int r=0; r<MAX_ACTIVE_LAYERS; r++) {

        if (_layer[r]) {
            delete _layer[r];
            _layer[r] = nullptr;
        }

        if (_tex[r]) {
            delete _tex[r];
            _tex[r] = nullptr;
        }
    }
}

void GameMap::setup(const int & x, const int & y, const int & Rx, const int & Ry) {

    _maxX = x;
    _maxY = y;
    _renderX = Rx;
    _renderY = Ry;

    _drawOffsetX = Rx;
    _drawOffsetY = Ry;

    _mapTiles = new TTile*[_maxX];

    for (int i=0; i<_maxX; ++i) {

        _mapTiles[i] = new TTile[_maxY];
    }

    for(int ax=0; ax<_maxX; ++ax){
    for(int ay=0; ay<_maxY; ++ay){

        //calculate first drawpositions
        if(ax%2){
            _mapTiles[ax][ay].pos_y = ay*94;
        }
        else{
            _mapTiles[ax][ay].pos_y = ay*94-47;
        }

        _mapTiles[ax][ay].pos_x = (ax*32)-32;

        for (int a=0; a<MAX_ACTIVE_LAYERS; ++a) {

            _mapTiles[ax][ay].tile_x[a] = 0; //rand() % 4;
            _mapTiles[ax][ay].tile_y[a] = 0;
            _mapTiles[ax][ay].active[a] = false;
        }

        _mapTiles[ax][ay].UnitLock = false;
        _mapTiles[ax][ay].tempLock = false;
        _mapTiles[ax][ay].Cost = 1; //always lowest cost

        _mapTiles[ax][ay].Lock = false;
        _mapTiles[ax][ay].isRoad = false;
        _mapTiles[ax][ay].AttackHEX = false;
        _mapTiles[ax][ay].CompareHEX = false;
    }}


    _mapwsk = _mapTiles;

    MAP_WIDTH = _maxX;
    MAP_HEIGHT = _maxY;

    AStarWay.clear();
}

void GameMap::resize(const int & x, const int & y) {

    int NX = x;
    int NY = y;

    TTile **NewTiles = nullptr;

    NewTiles = new TTile*[NX];

    for (int i=0; i<NX; ++i){

        NewTiles[i] = new TTile[NY];
    }

    for(int ax=0; ax<NX; ++ax){
    for(int ay=0; ay<NY; ++ay){

        if(ax%2){
            NewTiles[ax][ay].pos_y = ay*94;
        }
        else{
            NewTiles[ax][ay].pos_y = ay*94-47;
        }

        NewTiles[ax][ay].pos_x = (ax*32)-32;

        for(int a=0; a<MAX_ACTIVE_LAYERS; ++a){
            NewTiles[ax][ay].tile_x[a] = 0; //rand() % 4;
            NewTiles[ax][ay].tile_y[a] = 0;
            NewTiles[ax][ay].active[a] = false;
        }

        NewTiles[ax][ay].UnitLock = false;
        NewTiles[ax][ay].tempLock = false;
        NewTiles[ax][ay].Cost = 1; //always lowest cost

        NewTiles[ax][ay].Lock = false;
        NewTiles[ax][ay].isRoad = false;
        NewTiles[ax][ay].AttackHEX = false;
        NewTiles[ax][ay].CompareHEX = false;
    }}

    int minx = min(_maxX, NX);
    int miny = min(_maxY, NY);

    for (int cax=0; cax<minx; ++cax) {
    for (int cay=0; cay<miny; ++cay) {

        NewTiles[cax][cay] = _mapTiles[cax][cay];
    }}

    for (int i=0; i<_maxX; ++i) {
        delete [] _mapTiles[i];
        _mapTiles[i] = NULL;
    }

    delete [] _mapTiles;

    _mapTiles = NewTiles;

    _maxX = NX;
    _maxY = NY;

    _mapwsk = _mapTiles;

    MAP_WIDTH = _maxX;
    MAP_HEIGHT = _maxY;

    AStarWay.clear();
}

void GameMap::draw(const DrawingContext & context) {

    xVec2 cpos;
    xVec2 offset(32, 32);

    _offX = fmax((_sx/32.0), 0);
    _offY = fmax((_sy/94.0), 0);

#ifdef MAP_EDITOR
    _fx = (_drawOffsetX) + _offX + 1;
    _fy = (_drawOffsetY) + _offY + 1;
#else
    _fx = (_drawOffsetX) + _offX;
    _fy = (_drawOffsetY) + _offY;
#endif

    if(_fx > _maxX) _fx = _maxX;
    if(_fy > _maxY) _fy = _maxY;

    for(int ax=_offX; ax<_fx; ++ax){
    for(int ay=_offY; ay<_fy; ++ay){

        cpos = xVec2(_mapTiles[ax][ay].pos_x - _sx, _mapTiles[ax][ay].pos_y - _sy);
        cpos = (cpos * _scale) - (offset * (1.0-_scale)); //lerp

        _layer[0]->setAnimation(_mapTiles[ax][ay].tile_x[0], _mapTiles[ax][ay].tile_y[0]);
        _layer[0]->scale = _scale;
        _layer[0]->pos = cpos;
        _layer[0]->render(context);

#ifdef MAP_EDITOR

        if (!hideLock) {

            if(_mapTiles[ax][ay].Lock){

                _road->setAnimation(1, 0);
                _road->scale = _scale;
                _road->pos = cpos;
                _road->render(context);
            }
        }
#else

        _road->scale = _scale;
        _road->pos = cpos;

        if (_mapTiles[ax][ay].isRoad) {

            if (_mapTiles[ax][ay].Lock) {

                _road->setAnimation(1, 0);
            }
            else {

                _road->setAnimation(1, 1);
            }

            _road->render(context);
        }

        if (_mapTiles[ax][ay].AttackHEX) {

            _road->setAnimation(2, 0);
            _road->render(context);
        }
#endif

    }}

    cpos = ((_selectedPoint - xVec2(_sx, _sy)) * _scale) - (offset * (1.0 - _scale));
    _road->setAnimation(0, 0);
    _road->scale = _scale;
    _road->pos = cpos;
    _road->render(context);

    if (!AStarWay.empty()) {

        for (int i=0; i<AStarWay.size(); ++i) {

            auto p = xVec2(_mapTiles[(int)AStarWay[i].x][(int)AStarWay[i].y].pos_x,
                           _mapTiles[(int)AStarWay[i].x][(int)AStarWay[i].y].pos_y);

            cpos = ((p - xVec2(_sx,_sy)) * _scale) - (offset * (1.0-_scale));

            _road->setAnimation(0, 0);
            _road->scale = _scale;
            _road->pos = cpos;
            _road->render(context);
        }
    }

    if (_layer[1]) {

        for (int ax=_offX; ax<_fx; ax++) {
        for (int ay=_offY; ay<_fy; ay++) {

            if (_mapTiles[ax][ay].active[1]) {

                cpos = xVec2(_mapTiles[ax][ay].pos_x - _sx, _mapTiles[ax][ay].pos_y - _sy);
                cpos = (cpos * _scale) - (offset * (1.0 - _scale));

                _layer[1]->setAnimation(_mapTiles[ax][ay].tile_x[1], _mapTiles[ax][ay].tile_y[1]);
                _layer[1]->scale = _scale;
                _layer[1]->pos = cpos;
                _layer[1]->render(context);
            }
        }}
    }

    cpos = ((_testVec - xVec2(_sx, _sy)) * _scale) - (offset * (1.0 - _scale));

    _road->setAnimation(0, 1);
    _road->scale = _scale;
    _road->pos = cpos;
    _road->render(context);
}

xVec2 GameMap::getBias() {

    return xVec2(_sx, _sy);
}

xVec2 GameMap::getMapSize() {

    return xVec2(_maxX, _maxY);
}

xVec2 GameMap::getRenderSize() {

    return xVec2(_renderX, _renderY);
}

TTile** GameMap::getMap() {

    return _mapTiles;
}

float GameMap::getCurrendScale() {

    return _scale;
}

void GameMap::lockRoad() {

    for (int ax=0; ax<_maxX; ++ax) {
    for (int ay=0; ay<_maxY; ++ay) {

        if (_mapTiles[ax][ay].isRoad == false) {
        if (_mapTiles[ax][ay].UnitLock == false) {

            _mapTiles[ax][ay].tempLock = true;
        }}
    }}
}

void GameMap::cleanRoad() {

    for (int ax=0; ax<_maxX; ++ax) {
    for (int ay=0; ay<_maxY; ++ay) {

        _mapTiles[ax][ay].isRoad = false;
        _mapTiles[ax][ay].tempLock = false;
        _mapTiles[ax][ay].UnitLock = false;
        _mapTiles[ax][ay].AttackHEX = false;
        _mapTiles[ax][ay].CompareHEX = false;
    }}
}

void GameMap::cleanAStar() {

    if (!AStarWay.empty()) {

        _selectedPoint = xVec2(-128,-128);
        AStarWay.clear();
    }
}

bool GameMap::isPointEqual(const xVec2 & a, const xVec2 & b) {

    int tx = (int)_sx % 32;
    int ty = (int)_sy % 94;

    int ax = int((a.x+((16.0+tx)*_scale))/(32.0*_scale));
    int ay = int((a.y+((47.0+ty)*_scale))/(94.0*_scale));

    xVec2 Test1;
    xVec2 Test2;

    Test1.x = int(ax + (_sx/32.0));
    Test1.y = int(ay + (_sy/94.0));

    ax = int((b.x+((16.0+tx)*_scale))/(32.0*_scale));
    ay = int((b.y+((47.0+ty)*_scale))/(94.0*_scale));

    Test2.x = int(ax + (_sx/32.0));
    Test2.y = int(ay + (_sy/94.0));

    if ((Test1.x == Test2.x) && (Test1.y == Test2.y)) {

        return true;
    }
    else {

        return false;
    }
}

void GameMap::move(const float & mx, const float & my, float delta) {

    _sx = _sx + (mx * delta);
    _sy = _sy + (my * delta);

    _sx = floorf(_sx);
    _sy = floorf(_sy);

    if(_sx > (_maxX - (_drawOffsetX))*32.0) _sx = (_maxX - (_drawOffsetX))*32.0;
    if(_sy > (_maxY - (_drawOffsetY))*94.0) _sy = (_maxY - (_drawOffsetY))*94.0;

    if(_sx < 0) _sx = 0;
    if(_sy < 0) _sy = 0;
}

void GameMap::move(const xVec2 & npos) {

    _sx = npos.x;
    _sy = npos.y;

    _sx = floorf(_sx);
    _sy = floorf(_sy);

    if(_sx > (_maxX - (_drawOffsetX))*32.0) _sx = (_maxX - (_drawOffsetX))*32.0;
    if(_sy > (_maxY - (_drawOffsetY))*94.0) _sy = (_maxY - (_drawOffsetY))*94.0;

    if(_sx < 0) _sx = 0;
    if(_sy < 0) _sy = 0;
}

void GameMap::allowScaling(bool flag) {

    _scalable = flag;
}

void GameMap::setScale(const float & scale) {

    if(_scalable == false) return;

    _scale = scale;

    if(_scale > _maxs) _scale = _maxs;
    if(_scale < _mins) _scale = _mins;

    _drawOffsetX = (float(_renderX) * ((32.0/_scale)/32.0));
    _drawOffsetY = (float(_renderY) * ((94.0/_scale)/94.0));
}

void GameMap::setMinMaxScale(const float & min, const float & max) {

    if(_scalable == false) return;

    _mins = min;
    _maxs = max;
}

void GameMap::testPoint(const xVec2 & p) {

    _rx = (int)_sx % 32;
    _ry = (int)_sy % 94;

    int ax = int((p.x+((16.0+_rx)*_scale))/(32.0*_scale));
    int ay = int((p.y+((47.0+_ry)*_scale))/(94.0*_scale));
    int a = int(ax + ((_sx)/32.0));
    int b = int(ay + ((_sy)/94.0));

    if(a>_maxX-1) a=_maxX-1;
    if(b>_maxY-1) b=_maxY-1;
    if(a<0) a=0;
    if(b<0) b=0;

    _drawTile = xVec2(a,b);

    _testVec.x = _mapTiles[a][b].pos_x;
    _testVec.y = _mapTiles[a][b].pos_y;
}

void GameMap::selectTile(const xVec2 & p) {

    _rx = (int)_sx % 32;
    _ry = (int)_sy % 94;

    int ax = int((p.x+((16.0+_rx)*_scale))/(32.0*_scale));
    int ay = int((p.y+((47.0+_ry)*_scale))/(94.0*_scale));

    _nodeStart.x = int(ax + ((_sx)/32.0));
    _nodeStart.y = int(ay + ((_sy)/94.0));

    _selectedPoint.x = _mapTiles[_nodeStart.x][_nodeStart.y].pos_x;
    _selectedPoint.y = _mapTiles[_nodeStart.x][_nodeStart.y].pos_y;

    if (!AStarWay.empty()) {

        AStarWay.clear();
    }
}

bool GameMap::selectEndPoint(const xVec2 & p) {

    _rx = (int)_sx % 32;
    _ry = (int)_sy % 94;

    int ax = int((p.x+((16.0+_rx)*_scale))/(32.0*_scale));
    int ay = int((p.y+((47.0+_ry)*_scale))/(94.0*_scale));
    _nodeEnd.x = int(ax + (_sx/32.0));
    _nodeEnd.y = int(ay + (_sy/94.0));

    _endPoint.x = _mapTiles[_nodeEnd.x][_nodeEnd.y].pos_x;
    _endPoint.y = _mapTiles[_nodeEnd.x][_nodeEnd.y].pos_y;

    // Test if road
    if(!_mapTiles[_nodeEnd.x][_nodeEnd.y].isRoad) { //no? no need for A* test

        return false;
    }

    //ok its road but maybe its locked?
    if(_mapTiles[_nodeEnd.x][_nodeEnd.y].Lock) { //if locked do not perform A star

        return false;
    }

    //-------------------------------------------A STAR

    bool result = false;

    unsigned int SearchCount = 0;

    const unsigned int NumSearches = 1;

    //#define DEBUG_LISTS 1
    //#define DISPLAY_SOLUTION 1
    //#define DEBUG_LIST_LENGTHS_ONLY 1

    AStarWay.clear();

    while(SearchCount < NumSearches) {

        _astarsearch.SetStartAndGoalStates(_nodeStart, _nodeEnd);

        unsigned int SearchState;
        unsigned int SearchSteps = 0;

        do
        {
            SearchState = _astarsearch.SearchStep();

            SearchSteps++;

#if DEBUG_LISTS

            std::cout << "Steps: " << SearchSteps << std::endl;

            int len = 0;

            std::cout << "Open:" << std::endl;

            SearchNode *p = astarsearch.GetOpenListStart();

            while( p )
            {
                len++;
#if !DEBUG_LIST_LENGTHS_ONLY
                ((SearchNode *)p)->PrintNodeInfo();
#endif
                p = astarsearch.GetOpenListNext();

            }

            std::cout << "Open list has " << len << " nodes" << std::endl;

            len = 0;

            std::cout << "Closed: " << std::endl;

            //dnodes.clear();

            p = astarsearch.GetClosedListStart();

            while( p )
            {
                len++;
#if !DEBUG_LIST_LENGTHS_ONLY
                p->PrintNodeInfo();
#endif
                AStarWay.push_back(xVec2(p->x,p->y));

                p = astarsearch.GetClosedListNext();
            }

            std::cout << "Closed list has " << len << " nodes" << std::endl;
#endif

        }
        while( SearchState == AStarSearch<SearchNode>::SEARCH_STATE_SEARCHING );

        if( SearchState == AStarSearch<SearchNode>::SEARCH_STATE_SUCCEEDED ) {

            SearchNode *node = _astarsearch.GetSolutionStart();

#if DISPLAY_SOLUTION
            std::cout << "Displaying solution" << std::endl;
#endif
            int steps = 0;

            AStarWay.clear();

#if DISPLAY_SOLUTION
            node->PrintNodeInfo();
#endif
            AStarWay.push_back(xVec2(node->x,node->y));

            for( ;; )
            {
                node = _astarsearch.GetSolutionNext();

                if( !node )
                {
                    break;
                }
#if DISPLAY_SOLUTION
                node->PrintNodeInfo();
#endif
                AStarWay.push_back(xVec2(node->x,node->y));

                steps ++;

            };

            // Once you're done with the solution you can free the nodes up
            _astarsearch.FreeSolutionNodes();

            result = true;
        }
        else if( SearchState == AStarSearch<SearchNode>::SEARCH_STATE_FAILED )
        {
            std::cout << "Search terminated. Did not find goal state" << std::endl;

        }

        SearchCount ++;

        _astarsearch.EnsureMemoryFreed();
    }

    return result;
}

bool GameMap::canMoveFromTo(const xVec2 & start, const xVec2 & end) {

    _nodeStart.x = start.x;
    _nodeStart.y = start.y;
    _nodeEnd.x = end.x;
    _nodeEnd.y = end.y;

    if((int)_nodeStart.x < 0 ) _nodeStart.x = 0;
    if((int)_nodeStart.y < 0 ) _nodeStart.y = 0;
    if(_nodeStart.x >= MAP_WIDTH) _nodeStart.x = MAP_WIDTH-1;
    if(_nodeStart.y >= MAP_HEIGHT) _nodeStart.y = MAP_HEIGHT-1;

    if((int)_nodeEnd.x < 0 ) _nodeEnd.x = 0;
    if((int)_nodeEnd.y < 0 ) _nodeEnd.y = 0;
    if(_nodeEnd.x >= MAP_WIDTH) _nodeEnd.x = MAP_WIDTH-1;
    if(_nodeEnd.y >= MAP_HEIGHT) _nodeEnd.y = MAP_HEIGHT-1;

    _selectedPoint.x = _mapTiles[_nodeStart.x][_nodeStart.y].pos_x;
    _selectedPoint.y = _mapTiles[_nodeStart.x][_nodeStart.y].pos_y;

    _endPoint.x = _mapTiles[_nodeEnd.x][_nodeEnd.y].pos_x;
    _endPoint.y = _mapTiles[_nodeEnd.x][_nodeEnd.y].pos_y;

    if(!_mapTiles[_nodeEnd.x][_nodeEnd.y].isRoad) return false;
    if(_mapTiles[_nodeEnd.x][_nodeEnd.y].Lock) return false;

    bool result = false;

    unsigned int SearchCount = 0;

    const unsigned int NumSearches = 1;

    AStarWay.clear();

    while(SearchCount < NumSearches) {

        _astarsearch.SetStartAndGoalStates(_nodeStart, _nodeEnd);

        unsigned int SearchState;
        unsigned int SearchSteps = 0;

        do
        {
            SearchState = _astarsearch.SearchStep();

            SearchSteps++;

        }
        while( SearchState == AStarSearch<SearchNode>::SEARCH_STATE_SEARCHING );

        if( SearchState == AStarSearch<SearchNode>::SEARCH_STATE_SUCCEEDED ) {

            SearchNode *node = _astarsearch.GetSolutionStart();

            int steps = 0;

            AStarWay.clear();
            AStarWay.push_back(xVec2(node->x,node->y));

            for( ;; )
            {
                node = _astarsearch.GetSolutionNext();

                if( !node )
                {
                    break;
                }

                AStarWay.push_back(xVec2(node->x,node->y));

                steps ++;
            };

            // Once you're done with the solution you can free the nodes up
            _astarsearch.FreeSolutionNodes();

            result = true;
        }
        else if( SearchState == AStarSearch<SearchNode>::SEARCH_STATE_FAILED )
        {
            std::cout<< "Search terminated. Did not find goal state" << std::endl;
        }

        SearchCount ++;

        _astarsearch.EnsureMemoryFreed();
    }

    return result;
}

xVec2 GameMap::positionForSelection(const xVec2 & p) {

    int ax = p.x;
    int ay = p.y;

    if(ax < 0) ax = 0;
    if(ay < 0) ay = 0;
    if(ax >= MAP_WIDTH) ax = MAP_WIDTH-1;
    if(ay >= MAP_HEIGHT) ay = MAP_HEIGHT-1;

    if (!_mapTiles[ax][ay].Lock) {

        return xVec2(_mapTiles[ax][ay].pos_x, _mapTiles[ax][ay].pos_y);
    }
    else {

        return xVec2(0,0);
    }
}

xVec2 GameMap::selectionForPosition(const xVec2 & p) {

    xVec2 tabpos = xVec2(0,0);
    xVec2 cpos = p;

    for(int ax=0; ax<_maxX; ax++){
    for(int ay=0; ay<_maxY; ay++){

        xVec2 testp = cpos - xVec2(_mapTiles[ax][ay].pos_x, _mapTiles[ax][ay].pos_y);

        if ((fabs(testp.x) <= 2) && (fabs(testp.y) <= 2)) {

            tabpos = xVec2(ax,ay);
            break;
        }
    }}

    return tabpos;
}

xVec2 GameMap::hitTest() {

    return _testVec;
}

xVec2 GameMap::startPosition() {

    return _selectedPoint;
}

xVec2 GameMap::endPosition() {

    return _endPoint;
}

bool GameMap::canAttackAtTestPoint() {

    xVec2 sel = this->selectionForPosition(_testVec);
    return _mapTiles[(int)sel.x][(int)sel.y].AttackHEX;
}

bool GameMap::canMergeAtTestPoint() {

    xVec2 sel = this->selectionForPosition(_testVec);
    return _mapTiles[(int)sel.x][(int)sel.y].CompareHEX;
}

void GameMap::textureForLayer(const int & i, cFGLT* newTex) {

    if ( i < 0 ||  i > MAX_ACTIVE_LAYERS || !newTex) {
        std::cerr<< "TextureForLayer Wrong index or no Texture Attached. " << std::endl;
        return;
    }

    if(!_tex[i]){

        _tex[i] = newTex;
    }
    else{

        delete _tex[i];
        _tex[i] = nullptr;
        _tex[i] = newTex;
    }

    if (_layer[i]) {
        delete _layer[i];
        _layer[i] = nullptr;
    }

    _layer[i] = new Mesh2d(_tex[i]);
}

void GameMap::setTileForLayer(const int & i, const xVec2 & xy) {

    if (i < 0 || i > MAX_ACTIVE_LAYERS) {
        std::cerr<< "SetTile Wrong index" <<std::endl;
        return;
    }

#ifdef MAP_EDITOR

    if (penSize == 0) {

        int a = _drawTile.x;
        int b = _drawTile.y;

        if (i > 0) {
            _mapTiles[a][b].active[i] = true;
        }

        _mapTiles[a][b].tile_x[i] = xy.x;
        _mapTiles[a][b].tile_y[i] = xy.y;

        return;
    }

    xVec2 sp        =  _drawTile;
    xVec2 mapSize   = this->getMapSize();

    int STEP = penSize;
    int a = 0, b = 0;

    a = sp.x - STEP * 2;
    b = sp.y;

    int ax = 0;
    int ay = 0;

    for (int f = 0; f < STEP*2+1; ++f) {

        ax = a + (f*2);
        ay = b;

        if ((ax < 0)||(ax >= mapSize.x)) continue; //out of map?

        if (i > 0) { _mapTiles[ax][ay].active[i] = true; }

        _mapTiles[ax][ay].tile_x[i] = xy.x;
        _mapTiles[ax][ay].tile_y[i] = xy.y;
    }

    int Versor = (((int)sp.x % 2) ? 1 : -1);
    int d = 0;
    int e = 0;
    int g = 0;

    for (int c = 1; c < STEP+1; ++c) {

        d++;

        if (!(d%2)) e++;

        for (int f = 0; f < STEP*2+1-c; ++f) {

            ax = a + (f*2) + c;
            ay = b = sp.y-e*Versor;

            if ((ax < 0)||(ax >= mapSize.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapSize.y)) continue;

            if(i > 0) { _mapTiles[ax][ay].active[i] = true; }

            _mapTiles[ax][ay].tile_x[i] = xy.x;
            _mapTiles[ax][ay].tile_y[i] = xy.y;
        }

        if((d%2)) g++;

        //bottom test
        for (int f=0; f < STEP*2+1-c; ++f) {

            ax = a + (f*2) + c;
            ay = b = sp.y+g*Versor;

            if ((ax < 0)||(ax >= mapSize.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapSize.y)) continue;

            if (i > 0) { _mapTiles[ax][ay].active[i] = true; }

            _mapTiles[ax][ay].tile_x[i] = xy.x;
            _mapTiles[ax][ay].tile_y[i] = xy.y;
        }
    }

#endif

}

void GameMap::clearTile(const int & i) {

#ifdef MAP_EDITOR

    if (penSize == 0) {

        int a = _drawTile.x;
        int b = _drawTile.y;

        _mapTiles[a][b].active[i] = false;
        _mapTiles[a][b].tile_x[i] = 0;
        _mapTiles[a][b].tile_y[i] = 0;

        return;
    }

    xVec2 sp        = _drawTile;
    xVec2 mapsize   = this->getMapSize();

    int STEP = penSize;
    int a =0, b=0;

    a = sp.x - STEP * 2;
    b = sp.y;

    int ax = 0;
    int ay = 0;

    for (int f = 0; f < STEP*2+1; ++f) {

        ax = a + (f*2);
        ay = b;

        if ((ax < 0)||(ax >= mapsize.x)) continue; //out of map?

        _mapTiles[ax][ay].active[i] = false;
        _mapTiles[ax][ay].tile_x[i] = 0;
        _mapTiles[ax][ay].tile_y[i] = 0;
    }

    int Versor = (((int)sp.x % 2) ? 1 : -1);
    int d = 0;
    int e = 0;
    int g = 0;

    for (int c = 1; c < STEP+1; ++c) {
        d++;

        if(!(d%2)) e++;

        for(int f=0; f<STEP*2+1-c; f++){

            ax = a + (f*2) + c;
            ay = b = sp.y-e*Versor;

            if ((ax < 0)||(ax >= mapsize.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapsize.y)) continue;

            _mapTiles[ax][ay].active[i] = false;
            _mapTiles[ax][ay].tile_x[i] = 0;
            _mapTiles[ax][ay].tile_y[i] = 0;
        }

        if((d%2)) g++;

        //bottom test
        for (int f = 0; f < STEP*2+1-c; ++f) {

            ax = a + (f*2) + c;
            ay = b = sp.y+g*Versor;

            if ((ax < 0)||(ax >= mapsize.x)) continue; //out of map?
            if ((ay < 0)||(ay >= mapsize.y)) continue;

            _mapTiles[ax][ay].active[i] = false;
            _mapTiles[ax][ay].tile_x[i] = 0;
            _mapTiles[ax][ay].tile_y[i] = 0;
        }
    }

#endif
}

void GameMap::fitTextureForLayer(const int & i) {

    for (int ax=0; ax<_maxX; ax++) {
    for (int ay=0; ay<_maxY; ay++) {

        _mapTiles[ax][ay].tile_x[i] = 0;
        _mapTiles[ax][ay].tile_y[i] = 0;
    }}
}

void GameMap::lockHex() {

    int a = _drawTile.x;
    int b = _drawTile.y;

    _mapTiles[a][b].Lock = true;
}

void GameMap::unlockHex() {

    int a = _drawTile.x;
    int b = _drawTile.y;

    _mapTiles[a][b].Lock = false;
}

bool GameMap::load(const std::string & path) {

    std::fstream m_file;

    m_file.open(path.c_str(), std::fstream::in|std::fstream::binary);

    if (!m_file) {

        std::cerr << "Error Durning loading file: " << path << " File not found or data corrupted" <<std::endl;
        m_file.close();

        return false;
    }

    _mapwsk = nullptr;
    MAP_WIDTH = 0;
    MAP_HEIGHT = 0;
    _sx = 0;
    _sy = 0;

    int32_t HEDER = 0;

    m_file.read((char*)&HEDER, sizeof(HEDER));

    if (HEDER != HEADER_ID) {

        std::cerr << "This is not PIXFIGHT map file!" << std::endl;
        m_file.close();

        return false;
    }

    m_file.read((char*)&_scalable, sizeof(_scalable));
    m_file.read((char*)&_mins, sizeof(_mins));
    m_file.read((char*)&_maxs, sizeof(_maxs));

    _scale = 1.0f;

    for (int r = 0; r < MAX_ACTIVE_LAYERS; ++r) {

        if (_layer[r]) {
            delete _layer[r];
            _layer[r] = nullptr;
        }

        if (_tex[r]) {
            delete _tex[r];
            _tex[r] = nullptr;
        }
    }

    if (_mapTiles) {

        for (int i = 0; i < _maxX; ++i) {
            delete [] _mapTiles[i];
            _mapTiles[i] = nullptr;
        }

        delete [] _mapTiles;
        _mapTiles = nullptr;
    }

    m_file.read((char*)&_renderX, sizeof(_renderX));
    m_file.read((char*)&_renderY, sizeof(_renderY));

    m_file.read((char*)&_drawOffsetX, sizeof(_drawOffsetX));
    m_file.read((char*)&_drawOffsetY, sizeof(_drawOffsetY));

    m_file.read((char*)&_maxX, sizeof(_maxX));
    m_file.read((char*)&_maxY, sizeof(_maxY));

    _mapTiles = new TTile*[_maxX];

    for (int i = 0; i < _maxX; ++i) {

        _mapTiles[i] = new TTile[_maxY];
    }

    for (int ax = 0; ax<_maxX; ++ax) {
    for (int ay = 0; ay<_maxY; ++ay) {

        m_file.read((char*)&_mapTiles[ax][ay], sizeof(TTile));
    }}

    //read base tex

    TexStruct tmpStruct;
    char tname[256];
    int32_t lid = -1;

    cFGLT *mTex = nullptr;

    for (int i=0; i<MAX_ACTIVE_LAYERS; ++i) {

        m_file.read((char*)&lid, sizeof(lid));

        if (lid == -1) { continue; }

        m_file.read((char*)tname, 256*sizeof(char));
        m_file.read((char*)&tmpStruct, sizeof(TexStruct));

        mTex = new cFGLT;

        std::string p = _rootPath + tname + ".png";

        mTex->iTexture  = textureLoader.loadFile(p, GL_LINEAR, 0, GL_REPEAT, true);
        mTex->Width  = tmpStruct.Width;
        mTex->Height =  tmpStruct.Height;
        mTex->TileWidth = tmpStruct.TileWidth;
        mTex->TileHeight = tmpStruct.TileHeight;
        mTex->tiles_x = tmpStruct.tiles_x;
        mTex->tiles_y = tmpStruct.tiles_y;
        mTex->active = true;

        this->textureForLayer(i, mTex);

        mTex = nullptr;
    }

    for (int i = 0; i < MAX_PLAYERS_DEF; ++i) {
        
        m_file.read((char*)&_camPos[i], sizeof(xVec2));
    }

    _mapwsk = _mapTiles;

    MAP_WIDTH = _maxX;
    MAP_HEIGHT = _maxY;

    AStarWay.clear();
    m_file.close();

    return true;
}

bool GameMap::save(const std::string & path) {

    std::fstream m_file;

    m_file.open(path.c_str(), std::fstream::out|std::fstream::binary);

    if(!m_file){
        std::cerr << "Error Durning opening file: " << path << " propably no disk space" << std::endl;
        m_file.close();
        return false;
    }

    int32_t HEADER = HEADER_ID;

    m_file.write((const char*)&HEADER, sizeof(HEADER));

    //write scale info
    m_file.write((const char*)&_scalable, sizeof(_scalable));
    m_file.write((const char*)&_mins, sizeof(_mins));
    m_file.write((const char*)&_maxs, sizeof(_maxs));

    //initial scale is always 1.0 do not save

    // write initial data
    m_file.write((const char*)&_renderX, sizeof(_renderX));
    m_file.write((const char*)&_renderY, sizeof(_renderY));

    m_file.write((const char*)&_drawOffsetX, sizeof(_drawOffsetX));
    m_file.write((const char*)&_drawOffsetY, sizeof(_drawOffsetY));

    m_file.write((const char*)&_maxX, sizeof(_maxX));
    m_file.write((const char*)&_maxY, sizeof(_maxY));

    // write map stuff
    for (int ax=0; ax<_maxX; ++ax) {
    for (int ay=0; ay<_maxY; ++ay) {

        m_file.write((const char*)&_mapTiles[ax][ay], sizeof(TTile));
    }}

    //write map textures
    TexStruct tmpStruct;

    int32_t lid = -1;

    for (int i=0; i<MAX_ACTIVE_LAYERS; ++i) {

        if (_tex[i] == nullptr) {

            lid = -1;
            m_file.write((const char*)&lid, sizeof(lid));

            continue;
        }

        lid = i;
        m_file.write((const char*)&lid, sizeof(lid));

        const char *wsk = textureLoader.name(_tex[i]->iTexture);

        tmpStruct.Width      = _tex[i]->Width;
        tmpStruct.Height     = _tex[i]->Height;
        tmpStruct.TileWidth  = _tex[i]->TileWidth;
        tmpStruct.TileHeight = _tex[i]->TileHeight;
        tmpStruct.tiles_x    = _tex[i]->tiles_x;
        tmpStruct.tiles_y    = _tex[i]->tiles_y;

        m_file.write((const char*)wsk, 256*sizeof(char));
        m_file.write((const char*)&tmpStruct, sizeof(TexStruct));
    }

    for (int i=0; i < MAX_PLAYERS_DEF; ++i) {

        m_file.write((const char*)&_camPos[i], sizeof(xVec2));
    }

    m_file.close();

    return true;
}

void GameMap::saveCamForPlayer(const int & i) {

    if ( (i<0) && (i>MAX_PLAYERS_DEF) ) {
        std::cout << "Invalid PlayerID" <<std::endl;
        return;
    }

    _camPos[i] = xVec2(_sx, _sy);
}

void GameMap::loadCamForPlayer(const int & i) {

    if ( (i<0) && (i>MAX_PLAYERS_DEF) ) {
        std::cout << "Invalid PlayerID" <<std::endl;
        return;
    }

    _sx = _camPos[i].x;
    _sy = _camPos[i].y;
}

void GameMap::changeRenderOffset(const int & rx, const int & ry) {

    _renderX = rx;
    _renderY = ry;

    _drawOffsetX = rx;
    _drawOffsetY = ry;
}
