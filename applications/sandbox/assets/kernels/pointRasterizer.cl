
uint wang_hash(uint seed)
{
        seed = (seed ^ 61) ^ (seed >> 16);
        seed *= 9;
        seed = seed ^ (seed >> 4);
        seed *= 0x27d4eb2d;
        seed = seed ^ (seed >> 15);
        return seed;
}


float4 SampleTriangle(float2 rand,float4 a, float4 b, float4 c )
{
    float4 r;
    r= (float4)(a + rand.x*(b-a) + rand.y * (c-a));
    return r;
}

const int sampelePerTri=3;
__kernel void Main(__global const float* vertices,__global const uint* indices,__global const uint* seed, __global float4* points )
{
    int n=get_global_id(0)*3;
    int currentSeed = seed[n];
	//int n = get_global_id(0) *9;
    int resultIndex = get_global_id(0)*sampelePerTri;
    //get vertex indices
    uint vertex1Index = indices[n];
    uint vertex2Index = indices[n+1];
    uint vertex3Index = indices[n+2];

    //get vertex values
    //vert1
    float v1a = vertices[vertex1Index];
    float v1b = vertices[vertex1Index+1];
    float v1c = vertices[vertex1Index+2];
    float4 vertA = (float4)(v1a,v1b,v1c,1.0f);
    //vert2
    float v2a = vertices[vertex2Index];
    float v2b = vertices[vertex2Index+1];
    float v2c = vertices[vertex2Index+2];
    float4 vertB = (float4)(v2a,v2b,v2c,1.0f);
    //vert3
    float v3a = vertices[vertex3Index];
    float v3b = vertices[vertex3Index+1];
    float v3c = vertices[vertex3Index+2];
    float4 vertC = (float4)(v3a,v3b,v3c,1.0f);

    for(int i =0; i <sampelePerTri; i++)
    {
        int index= resultIndex + i;
     //   float a = wang_hash(currentSeed) /(float)UINT_MAX;
        float2 randPoint =(float2)( wang_hash(currentSeed) /(float)UINT_MAX, wang_hash(currentSeed+1) /(float)UINT_MAX);
        currentSeed+=2;

        float4 newPoint = SampleTriangle(randPoint,vertA,vertB,vertC);
        newPoint=(float4)(vertices[0]+i, vertices[1]+i, vertices[2]+i,1);
       // newPoint=vertC;
       // newPoint= (float4)(randPoint.x,randPoint.y,0,1);
       // float4 newPoint = (float4)(wang_hash(currentSeed) /(float)UINT_MAX,0,0,n/3);
        points[index]=newPoint;
        currentSeed++;
    }

    // //Vertex1
    // points[n] =  vertices[n];
    // points[n+1] = vertices[n+1];
    // points[n+2] = vertices[n+2];

    // //Vertex2
    // points[n+3] =  vertices[n+3];
    // points[n+4] = vertices[n+4];
    // points[n+5] = vertices[n+5];
    // //Vertex3
    // points[n+6] =  vertices[n+6];
    // points[n+7] = vertices[n+7];
    // points[n+8] = vertices[n+8];
    
}

