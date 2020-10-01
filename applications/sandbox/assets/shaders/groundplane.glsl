#version 450
#state CULL OFF

layout(location = SV_POSITION)    in vec3 vertex;
layout(location = SV_MODELMATRIX) in mat4 modelMatrix;

layout(location = SV_VIEW)    uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

out TOGEOM
{
    vec3 position;
} outData;

void vert(void)
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(vertex, 1.f);
    outData.position = vertex;
}

layout(triangles)in;
layout(triangle_strip, max_vertices = 3)out;

in TOGEOM
{
    vec3 position;
} inData[];

out IO
{
    vec3 barycentricCoords;
    vec3 position;
} outData;

void geom(void)
{
    gl_Position = gl_in[0].gl_Position;
    outData.barycentricCoords = vec3(1, 0, 0);
    outData.position = inData[0].position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    outData.barycentricCoords = vec3(0, 1, 0);
    outData.position = inData[1].position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    outData.barycentricCoords = vec3(0, 0, 1);
    outData.position = inData[2].position;
    EmitVertex();
    EndPrimitive();
}

in IO
{
    vec3 barycentricCoords;
    vec3 position;
} inData;

out vec4 fragment_color;

void frag(void)
{
    vec3 deltas = fwidth(inData.barycentricCoords);
    vec3 adjustedCoords = smoothstep(deltas, 2* deltas, inData.barycentricCoords);
    float linePresence = min(adjustedCoords.x, min(adjustedCoords.y, adjustedCoords.z));
    if(linePresence > 0.9)
        discard;

    fragment_color = vec4(vec3(0), mix(1, 0, length(inData.position)/250.f));
}
