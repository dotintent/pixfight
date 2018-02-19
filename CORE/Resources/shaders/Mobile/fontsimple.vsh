//
//  Shader.vsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

#version 100

attribute vec4 coord;
varying vec2 texcoord;

void main()
{
    gl_Position = vec4(coord.xy, 0, 1);
    texcoord = coord.zw;
}
