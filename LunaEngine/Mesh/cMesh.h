#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <vector>
#include <string>
#include <glm/vec3.hpp>

static const unsigned int NUMBEROFBONES = 4;

// structure of loaded data
struct sMeshVertex
{
	sMeshVertex():
		x(0.f), y(0.f), z(0.f), w(1.f),
		nx(0.f), ny(0.f), nz(0.f), nw(1.f),
		u(0.f), v(0.f),
		tx(0.f), ty(0.f), tz(0.f), tw(1.f),
		bx(0.f), by(0.f), bz(0.f), bw(1.f)
	{
		memset(this->boneID, 0, sizeof(unsigned int) * NUMBEROFBONES);
		memset(this->boneWeights, 0, sizeof(float) * NUMBEROFBONES);
	}
	float x, y, z, w;
	float nx, ny, nz, nw;
	float u, v;
	float tx, ty, tz, tw;	// tangents
	float bx, by, bz, bw;	// bi-tangents
	float boneID[4];		// which bone impacts this vertex
	float boneWeights[4];	// weights for bones
};

struct sTriangle
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

	std::string name;
	bool isAnimated = false;

	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(FLT_MIN);

	std::vector<sMeshVertex> vecVertices;
	std::vector<sTriangle> vecTriangles;
	std::vector<sMeshTriangle> vecMeshTriangles;

};

#endif 
