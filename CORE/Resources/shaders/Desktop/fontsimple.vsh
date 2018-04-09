#version 150

in vec4 coord;
out vec2 texcoord;

void main()
{
    texcoord = coord.zw;
    gl_Position = vec4(coord.xy, 0.0, 1.0);
}
