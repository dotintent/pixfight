//
//  SearchNode.cpp
//  PixFight
//
//  Created by Marcin Małysz on 05/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "SearchNode.hpp"
#include <math.h>

TTile** _mapwsk = nullptr;
int MAP_WIDTH = 0;
int MAP_HEIGHT = 0;

int GetMap( int x, int y ) {

    if ( x < 0 || x >= (MAP_WIDTH-1) ||
         y < 0 || y >= (MAP_HEIGHT-1) ) {

        return 9;
    }

    return (_mapwsk[x][y].Lock || _mapwsk[x][y].tempLock) ? 9 : _mapwsk[x][y].Cost;
}

bool SearchNode::IsSameState( SearchNode &rhs ) {

    // same state in a maze search is simply when (x,y) are the same
    if ( (x == rhs.x) && (y == rhs.y)) {

        return true;
    }
    else {

        return false;
    }
}

void SearchNode::PrintNodeInfo() {

    std::cout<<"Node position : ( " << x << ", " << y << ")" << std::endl;
}

// Here's the heuristic function that estimates the distance from a Node
// to the Goal.

float SearchNode::GoalDistanceEstimate( SearchNode &nodeGoal ) {

    float xd = fabs(float(((float)x - (float)nodeGoal.x)));
    float yd = fabs(float(((float)y - (float)nodeGoal.y)));

    return sqrtf(xd + yd);
}

bool SearchNode::IsGoal( SearchNode &nodeGoal ) {

    if ((x == nodeGoal.x) && (y == nodeGoal.y)) {

        return true;
    }

    return false;
}

// This generates the successors to the given Node. It uses a helper function called
// AddSuccessor to give the successors to the AStar class. The A* specific initialisation
// is done for each node internally, so here you just set the state information that
// is specific to the application
bool SearchNode::GetSuccessors( AStarSearch<SearchNode> *astarsearch, SearchNode *parent_node )
{

    int parent_x = -1;
    int parent_y = -1;

    if( parent_node )
    {
        parent_x = parent_node->x;
        parent_y = parent_node->y;
    }

    SearchNode NewNode;

    // push each possible move except allowing the search to go backwards

    if ((GetMap( x+1, y ) < 9) && !((parent_x == x+1) && (parent_y == y))) {

        NewNode = SearchNode( x+1, y);
        astarsearch->AddSuccessor( NewNode );
    }

    if ((GetMap( x-1, y) < 9) && !((parent_x == x-1) && (parent_y == y))) {

        NewNode = SearchNode( x-1, y);
        astarsearch->AddSuccessor( NewNode );
    }

    if (x % 2) {
        //not even

        if ((GetMap( x+1, y+1 ) < 9) && !((parent_x == x+1) && (parent_y == y+1))) {

            NewNode = SearchNode( x+1, y+1);
            astarsearch->AddSuccessor( NewNode );
        }

        if ((GetMap( x-1, y+1) < 9) && !((parent_x == x-1) && (parent_y == y+1))) {

            NewNode = SearchNode( x-1, y+1);
            astarsearch->AddSuccessor( NewNode );
        }
    }
    else {
        //even

        if ((GetMap( x+1, y-1 ) < 9) && !((parent_x == x+1) && (parent_y == y-1))) {

            NewNode = SearchNode( x+1, y-1);
            astarsearch->AddSuccessor( NewNode );
        }

        if ((GetMap( x-1, y-1) < 9) && !((parent_x == x-1) && (parent_y == y-1))) {

            NewNode = SearchNode( x-1, y-1);
            astarsearch->AddSuccessor( NewNode );
        }
    }

    if ((GetMap( x+2, y ) < 9) && !((parent_x == x+2) && (parent_y == y))) {

        NewNode = SearchNode( x+2, y );
        astarsearch->AddSuccessor( NewNode );
    }

    if ((GetMap( x-2, y ) < 9) && !((parent_x == x-2) && (parent_y == y))) {

        NewNode = SearchNode( x-2, y );
        astarsearch->AddSuccessor( NewNode );
    }

    return true;
}

// given this node, what does it cost to move to successor. In the case
// of our map the answer is the map terrain value at this node since that is
// conceptually where we're moving

float SearchNode::GetCost(SearchNode & successor) {

    return (float)GetMap( x, y );
}
