#vertex

#version 330 core
layout(location = 0) in vec3 pos;

void main() { gl_Position = vec4(pos.xyz, 1.0); }

#fragment

#version 330 core
in vec4 gl_FragCoord;

out vec4 frag_color;

void main() { frag_color = vec4(1.0, 0.0, 1.0, 1.0); }
