#version 450

layout(location = 1) in vec4 color;
layout(location = SV_POSITION) in vec3 vertex;
layout(location = SV_VIEW) uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

layout(location = 0) out vec4 outColor;

void vert(void)
{
    gl_Position = (projectionMatrix * viewMatrix) * vec4(vertex, 1.f);
    outColor = color;
}

layout(location = 0) in vec4 color;
out vec4 fragment_color;

void frag(void)
{
    fragment_color = color;
}
