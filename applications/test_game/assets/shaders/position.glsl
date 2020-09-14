#version 450

layout(location = SV_POSITION)in vec3 vertex;
layout(location = SV_MODELMATRIX)in mat4 modelMatrix;
uniform	mat4 viewProjectionMatrix;

out vec3 position;

void vert(void)
{
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(vertex, 1.f);
    position = (modelMatrix * vec4(vertex, 1.f)).xyz;
}

in vec3 position;
out vec4 fragment_color;

void frag(void)
{
    fragment_color = vec4(position, 1);
}
