#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <vector>		// "smart array"

// This structure matches the file, so that our life is
//   simpler and happier, and we all get along.
struct sPlyVertexXYZ
{
	float x, y, z;
};

struct sPlyTriangle
{
	unsigned int vert_index_1;
	unsigned int vert_index_2;
	unsigned int vert_index_3;
};

class cMesh
{
public:
	cMesh() {};			// Empty c'tor
	~cMesh() {};		// Empty d'tor
	// Store the vertices
	std::vector<sPlyVertexXYZ> vecVertices;

	// Store the triangles
	std::vector<sPlyTriangle> vecTriangles;

};

#endif 
