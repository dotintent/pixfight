#version 150

uniform sampler2D tex;
uniform vec4 rcolor;

in vec2 tcoord;
out vec4 fragColor; 

void main()
{
    vec4 outColor = texture(tex,tcoord.st,0.0);
    fragColor = outColor * rcolor;
}
