//
//  AIAware.hpp
//  PixFight
//
//  Created by Marcin Małysz on 07/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

class AIAware {
public:

    virtual int getUniqueID() = 0;
    virtual int getRequestID() = 0;
    virtual int getTeamID() = 0;
    virtual bool isBase() = 0;

    virtual void setUnitMode(const UNITMODE & mode) = 0;
    virtual void setRequestID(const int & requestid) = 0;
    virtual void setUniqueID(const int & uniqueid) = 0;
};
