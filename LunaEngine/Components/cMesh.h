#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <vector>
#include <glm/vec3.hpp>


// structure of loaded data
struct sPlyVertexXYZ
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	float bi_x, bi_y, bi_z;
	float tan_x, tan_y, tan_z;
	float boneID[4];		// which bone impacts this vertex
	float boneWeights[4];	// weights for bones
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

	glm::vec3 m1;	// Mid-Vertex 1
	glm::vec3 m2;	// Mid-Vertex 2
	glm::vec3 m3;	// Mid-Vertex 3
	glm::vec3 m;	// Mid-Vertex Global
};

class cMesh
{
public:
	cMesh() {};
	virtual ~cMesh() {};

	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(FLT_MIN);

	std::vector<sPlyVertexXYZ> vecVertices;
	std::vector<sPlyTriangle> vecTriangles;
	std::vector<sMeshTriangle> vecMeshTriangles;

};

#endif 
