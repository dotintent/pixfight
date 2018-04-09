#version 150

in vec4 position;
in vec2 coord;

out vec2 tcoord;

uniform	mat4 projection;
uniform mat4 modelView;

uniform vec4 tpos;

void main()
{
    tcoord.st = coord.st;
    tcoord.s += tpos.x*tpos.z;
    tcoord.t += tpos.y*tpos.w;
    
	gl_Position = projection * modelView * position;
    
}
