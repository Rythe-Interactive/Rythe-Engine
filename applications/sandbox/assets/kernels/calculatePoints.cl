


__kernel void Main
(
    __global const float* vertices,
    __global const uint* indices,
    const uint samplesPerTri,
    __global uint* pointsCount
)
{
    //init indices and rand state
    int n=get_global_id(0)*3;
    int triangelIndex= get_global_id(0);


    //get vertex values and corrosponding uvs
    //vertA
    float v1a = vertices[indices[n]*3];
    float v1b = vertices[indices[n]*3+1];
    float v1c = vertices[indices[n]*3+2];
    float4 vertA = (float4)(v1a,v1b,v1c,1.0f);

    //vert2
    float v2a = vertices[indices[n+1]*3];
    float v2b = vertices[indices[n+1]*3+1];
    float v2c = vertices[indices[n+1]*3+2];
    float4 vertB = (float4)(v2a,v2b,v2c,1.0f);

    //vert3
    float v3a = vertices[indices[n+2]*3];
    float v3b = vertices[indices[n+2]*3+1];
    float v3c = vertices[indices[n+2]*3+2];
    float4 vertC = (float4)(v3a,v3b,v3c,1.0f);


    //get the size of the triangle
    //size is used to modify the amount of samples for the current triangle
    //large triangles should have more samples than a very small triangle
    float lengthA = length(vertC-vertA);
    float lengthB = length(vertB-vertA);
    float lengthC = length(vertC-vertB);
    float size = lengthA+lengthB+lengthC;

    int newSampleCount = ceil(size * samplesPerTri);

    pointsCount[triangelIndex] = newSampleCount;
}

