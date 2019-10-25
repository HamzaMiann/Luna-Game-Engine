#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <vector>		// "smart array"
#include <glm/vec3.hpp>

// This structure matches the file, so that our life is
//   simpler and happier, and we all get along.
struct sPlyVertexXYZ
{
	float x, y, z;
	float nx, ny, nz;
};

struct sPlyTriangle
{
	unsigned int vert_index_1;
	unsigned int vert_index_2;
	unsigned int vert_index_3;
};

struct sMeshTriangle
{
	glm::vec3 first;
	glm::vec3 second;
	glm::vec3 third;
	glm::vec3 normal;
};

class cMesh
{
public:
	cMesh() {};			// Empty c'tor
	~cMesh() {};		// Empty d'tor

	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(FLT_MIN);

	// Store the vertices
	std::vector<sPlyVertexXYZ> vecVertices;

	// Store the triangles
	std::vector<sPlyTriangle> vecTriangles;

	std::vector<sMeshTriangle> vecMeshTriangles;

};

#endif 
