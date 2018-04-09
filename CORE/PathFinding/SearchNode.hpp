//
//  SearchNode.hpp
//  PixFight
//
//  Created by Marcin Małysz on 05/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "stlastar.hpp"
#include "MapTile.hpp"

extern TTile** _mapwsk;
extern int GetMap(int x, int y);
extern int MAP_WIDTH, MAP_HEIGHT;

class SearchNode {

public:

    // the (x,y) positions of the node
    unsigned int x;
    unsigned int y;

    SearchNode() { x = y = 0; }
    SearchNode( unsigned int px, unsigned int py ) { x = px; y = py; }

    float GoalDistanceEstimate( SearchNode &nodeGoal );
    bool IsGoal( SearchNode &nodeGoal );
    bool GetSuccessors( AStarSearch<SearchNode> *astarsearch, SearchNode *parent_node );
    float GetCost( SearchNode &successor );
    bool IsSameState( SearchNode &rhs );

    void PrintNodeInfo();
};
