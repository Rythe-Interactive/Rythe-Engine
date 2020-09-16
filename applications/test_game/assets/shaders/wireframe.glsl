#version 450
#enable CULL OFF

layout(location = SV_POSITION)in vec3 vertex;
layout(location = SV_MODELMATRIX)in mat4 modelMatrix;

layout(location = SV_VIEW) uniform mat4 viewMatrix;
layout(location = SV_PROJECT) uniform mat4 projectionMatrix;

void vert(void)
{
    gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * vec4(vertex, 1.f);
}

layout(triangles)in;
layout(triangle_strip, max_vertices = 3)out;

out vec3 barycentricCoords;

void geom(void)
{
    gl_Position = gl_in[0].gl_Position;
    barycentricCoords = vec3(1, 0, 0);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    barycentricCoords = vec3(0, 1, 0);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    barycentricCoords = vec3(0, 0, 1);
    EmitVertex();
    EndPrimitive();
}

in vec3 barycentricCoords;
out vec4 fragment_color;

void frag(void)
{
    vec3 deltas = fwidth(barycentricCoords);
    vec3 adjustedCoords = smoothstep(deltas, 2* deltas, barycentricCoords);
    float linePresence = min(adjustedCoords.x, min(adjustedCoords.y, adjustedCoords.z));
    if(linePresence > 0.9)
        discard;

    fragment_color = vec4(vec3(0), 1);
}
