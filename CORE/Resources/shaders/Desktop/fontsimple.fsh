#version 150

uniform sampler2D tex;
uniform vec4 color;

in vec2 texcoord;
out vec4 fragColor;

void main() {
    
    fragColor = vec4(1.0, 1.0, 1.0, texture(tex, texcoord, 0.0).r) * color;
}
