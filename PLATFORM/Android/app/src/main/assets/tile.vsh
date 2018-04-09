//
//  Shader.vsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

#version 100

attribute vec4 position;
attribute vec2 coord;

uniform	mat4 projection;
uniform mat4 modelView;

varying mediump vec2 tcoord;
uniform mediump vec4 tpos;

void main()
{
    tcoord.st = coord.st;
    tcoord.s += tpos.x*tpos.z;
    tcoord.t += tpos.y*tpos.w;
    
	gl_Position = projection*modelView * position;
    
}
