#version 450

layout(location = SV_POSITION)    in vec3 vertex;
layout(location = SV_MODELMATRIX) in mat4 modelMatrix;
layout(location = SV_VIEW)    uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

out IO
{
    vec3 position;
} outData;

void vert(void)
{
    outData.position = vertex;

    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(vertex, 1.f);
}

in IO
{
    vec3 position;
} inData;

out vec4 fragment_color;

void frag(void)
{
    fragment_color = vec4(inData.position, 1);
}
