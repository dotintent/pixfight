//
//  Drawable.hpp
//  PixFight
//
//  Created by Marcin Małysz on 07/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "DrawingContext.hpp"

class Drawable {

public:

    virtual void draw(const DrawingContext & context) = 0;
    virtual void setScale(const float & scale) { _scale = scale; }

protected:

    float _scale;
};
