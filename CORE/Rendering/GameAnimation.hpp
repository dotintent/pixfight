//
//  GameAnimation.hpp
//  PixFight
//
//  Created by Marcin Małysz on 15/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "Mesh2d.hpp"

class GameAnimation {

public:

    GameAnimation(const std::string & texpath,
                  const xVec2 & pos,
                  const xVec2 & size,
                  const xVec2 & tileSize,
                  const float & lenght,
                  const float & speed);

    ~GameAnimation() noexcept;

    void Draw(const DrawingContext & context, xVec2 bias);
    void setScale(const float & scale);
    void setRotation(const float & rotation);
    void setPosition(const xVec2 & position);

    void begin();
    bool end();
    void update(const float & dt);

private:

    bool _running;
    Mesh2d *_mesh;
    xVec2 _pos;

    int _tx;
    int _ty;
    int _mx;
    int _my;

    int _animsum;
    int _currentsum;

    float _speed;
    xVec2 _outPos;

    xVec2 _AABB;
    cFGLT _Tex;
    float _radius;
    float _accum;
    float _scale;
    float _rot;
};
