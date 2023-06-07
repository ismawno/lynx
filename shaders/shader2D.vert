#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 frag_color;

layout(push_constant) uniform Push
{
    mat4 transform;
    vec3 color;
}
push;

void main()
{
    gl_Position = push.transform * vec4(position, 0.0, 1.0);
    frag_color = color;
}