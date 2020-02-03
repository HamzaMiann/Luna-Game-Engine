#ifndef _cMesh_HG_
#define _cMesh_HG_

// This object represents the RAW MESH, as close to the original format as possible. 
// So the loader creates one of these, which is passed into the GPU (Vertex Buffer)
// You can keep this around if you want to update it. 
// The sModelDrawInfo structure contains this (for updating, if needed)

#include "sVertex_xyz_rgba_n_uv2_bt_4Bones.h"

//#define MAX_VERTEX_ARRAY_SIZE 2048
//#define MAX_INDEX_ARRAY_SIZE 2048
//static const int MAX_VERTEX_ARRAY_SIZE = 4096;
//static const int MAX_INDEX_ARRAY_SIZE = 4096;

#include <string>
#include <glm/glm.hpp>		
#include <glm/vec3.hpp>

// Represents a triangle inside the indexed mesh
class cTriangle
{
public:
	cTriangle() : vertex_ID_0(0), vertex_ID_1(0), vertex_ID_2(0) {};
	~cTriangle() {};
	int vertex_ID_0;
	int vertex_ID_1;
	int vertex_ID_2;
};

class cMesh
{
public:
	cMesh();
	~cMesh();

	std::string name;	// the friendly name of the mesh

	// Array of vertices 
//	cVertex_xyz_rgb vertices[MAX_VERTEX_ARRAY_SIZE]; // Stack
//	cVertex_xyz_rgb_n* pVertices;						// Heap
//	sVertex_xyz_rgba_n_uv2_bt* pVertices;				// Heap
	sVertex_xyz_rgba_n_uv2_bt_4Bones* pVertices;
	unsigned int numberOfVertices;

	// Array of triangles (indices)
//	cTriangle triangles[MAX_INDEX_ARRAY_SIZE];	// Stack
	cTriangle* pTriangles;						// Heap
	unsigned int numberOfTriangles;

	unsigned int* pIndices;
	unsigned int numberOfIndices;

	// Takes an indexed model and makes just a vertex array model
	// WARNING: Will OVERWRITE the vertex array 
//	void FlattenIndexedModel(void);

	// First 2 are the "bounding box" edges
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
	// This is Max - Min for each x,y,z
	glm::vec3 maxExtentXYZ;	
	float maxExtent;		// Biggest of above
	// Scale can be 1.0 div maxExtent;
	float scaleForUnitBBox;

	void CalculateExtents(void);

	void CalculateNormals(void);

	glm::vec3 CalculateClosestPointOnMesh( glm::vec3 pointToTest );

	glm::vec3 ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

};


#endif
