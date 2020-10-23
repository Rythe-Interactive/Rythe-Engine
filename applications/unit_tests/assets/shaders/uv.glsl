#version 450

layout(location = SV_POSITION)    in vec3 vertex;
layout(location = SV_TEXCOORD0)   in vec2 uv;
layout(location = SV_MODELMATRIX) in mat4 modelMatrix;
layout(location = SV_VIEW)    uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

out IO
{
    vec2 uv;
} outData;

void vert(void)
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(vertex, 1.f);
    outData.uv = uv;
}

in IO
{
    vec2 uv;
} inData;

out vec4 fragment_color;

void frag(void)
{
    fragment_color = vec4(inData.uv, 0, 1);
}
