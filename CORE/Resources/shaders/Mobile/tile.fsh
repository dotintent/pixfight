//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

#version 100

uniform mediump sampler2D tex;
varying mediump vec2 tcoord;
uniform mediump vec4 rcolor;

void main()
{
    lowp vec4 outColor = texture2D(tex,tcoord.st);
    gl_FragColor =  outColor * rcolor; //(outColor * rcolor.a) * (rcolor * rcolor.a);
}
