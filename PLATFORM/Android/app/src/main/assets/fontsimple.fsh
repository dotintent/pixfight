//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

#version 100

precision mediump float;

varying vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;

void main(void) {
    gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;
}
