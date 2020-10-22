#version 450

layout(location = SV_POSITION)    in vec3 vertex;
layout(location = SV_NORMAL)      in vec3 normal;
layout(location = SV_TEXCOORD0)   in vec2 uv;
layout(location = SV_MODELMATRIX) in mat4 modelMatrix;
layout(location = SV_VIEW)    uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

out IO
{
    vec3 normal;
} outData;

void vert(void)
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(vertex, 1.f);
    //outData.normal = (modelMatrix * vec4(normal, 0)).xyz;
    outData.normal = normal;
}

in IO
{
    vec3 normal;
} inData;

out vec4 fragment_color;

void frag(void)
{
    fragment_color = vec4((abs(inData.normal) + clamp(inData.normal, 0.0, 1.0) * 2) / 3, 1);
}
