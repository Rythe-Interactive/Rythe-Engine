#version 450
#state CULL OFF

layout(location = SV_POSITION)    in vec3 vertex;
layout(location = SV_NORMAL)      in vec3 normal;
layout(location = SV_TEXCOORD0)   in vec2 uv;
layout(location = SV_MODELMATRIX) in mat4 modelMatrix;
layout(location = SV_VIEW)    uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

out IO
{
    vec3 normal;
    vec2 uv;
    vec3 position;
} outData;

void vert(void)
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(vertex, 1.f);
    //outData.normal = (modelMatrix * vec4(normal, 0)).xyz;
    outData.normal = normal;
    outData.uv = uv;
    outData.position = vertex;
}

in IO
{
    vec3 normal;
    vec2 uv;
    vec3 position;
} inData;

out vec4 fragment_color;

void frag(void)
{
    vec3 unitDir = normalize(inData.position);

    fragment_color = vec4(mix(vec3(1.f), vec3(0.2f, 0.4f, 1.0f), clamp(dot(unitDir, vec3(0.f, 1.f, 0.f)) + 0.5f, 0.f, 1.f)), 1.f);
}
