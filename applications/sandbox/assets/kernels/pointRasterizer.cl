__kernel void Main(__global const float* vertices, __global float* points )
{
	int n = get_global_id(0) *9;
 
    //Vertex1
    points[n] =  vertices[n];
    points[n+1] = vertices[n+1];
    points[n+2] = vertices[n+2];

    //Vertex2
    points[n+3] =  vertices[n+3];
    points[n+4] = vertices[n+4];
    points[n+5] = vertices[n+5];
    //Vertex3
    points[n+6] =  vertices[n+6];
    points[n+7] = vertices[n+7];
    points[n+8] = vertices[n+8];
    
}