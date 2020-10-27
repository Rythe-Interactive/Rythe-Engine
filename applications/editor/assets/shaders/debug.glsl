#version 450

in vec4 color;
layout(location = SV_POSITION) in vec3 vertex;
layout(location = SV_VIEW)    uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

out IO
{
    vec4 color;
} outData;

void vert(void)
{
    gl_Position = (projectionMatrix * viewMatrix) * vec4(vertex, 1.f);
    outData.color = color;
}

in IO
{
    vec4 color;
} inData;

out vec4 fragment_color;

void frag(void)
{
    fragment_color = inData.color;
}
