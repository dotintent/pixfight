//
//  BasePathExtraction.cpp
//  PixFight
//
//  Created by Marcin Małysz on 03/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "BasePathExtraction.hpp"

struct MatchPathSeparator {
    
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};

std::string basename( std::string const& pathname ) {
    
    return std::string( std::find_if( pathname.rbegin(), pathname.rend(), MatchPathSeparator() ).base(),
                        pathname.end() );
}

std::string removeExtension( std::string const& filename ) {
    
    std::string::const_reverse_iterator pivot = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend() ? filename : std::string( filename.begin(), pivot.base() - 1 );
}
