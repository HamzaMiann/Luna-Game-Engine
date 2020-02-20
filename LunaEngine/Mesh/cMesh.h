#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <vector>
#include <string>
#include <map>
#include <glm/glm_common.h>

static const unsigned int NUMBEROFBONES = 4;

// structure of loaded data
struct sMeshVertex
{
	sMeshVertex():
		x(0.f), y(0.f), z(0.f), w(1.f),
		nx(0.f), ny(0.f), nz(0.f), nw(1.f),
		u(0.f), v(0.f),
		tx(0.f), ty(0.f), tz(0.f), tw(1.f),
		bx(0.f), by(0.f), bz(0.f), bw(1.f),
		index(0)
	{
		memset(this->boneID, 0.f, sizeof(float) * NUMBEROFBONES);
		memset(this->boneWeights, 0.f, sizeof(float) * NUMBEROFBONES);
	}
	float x, y, z, w;
	float nx, ny, nz, nw;
	float u, v;
	float tx, ty, tz, tw;	// tangents
	float bx, by, bz, bw;	// bi-tangents
	float boneID[NUMBEROFBONES];		// which bone impacts this vertex
	float boneWeights[NUMBEROFBONES];	// weights for bones

	unsigned int index;
	void SetID(float id, float weight) { boneID[index] = id; boneWeights[index++] = weight; }
};

struct sTriangle
{
	unsigned int vert_index_1;
	unsigned int vert_index_2;
	unsigned int vert_index_3;
};

struct sMeshTriangle
{
	vec3 first;
	vec3 second;
	vec3 third;
	vec3 normal;

	vec3 m1;	// Mid-Vertex 1
	vec3 m2;	// Mid-Vertex 2
	vec3 m3;	// Mid-Vertex 3
	vec3 m;	// Mid-Vertex Global
};

struct sHeirarchy
{
	std::string name;
	unsigned int id;
	mat4 transform;
	mat4 boneOffset;
	mat4 globalInverse;
	std::vector<sHeirarchy*> children;
};

class cMesh
{
public:
	cMesh() {};
	virtual ~cMesh() {};

	std::string name;
	bool isAnimated = false;

	unsigned int numBones = 0;

	vec3 min = vec3(FLT_MAX);
	vec3 max = vec3(FLT_MIN);

	std::map<std::string, unsigned int> boneIDs;
	sHeirarchy* root = nullptr;

	std::vector<sMeshVertex> vecVertices;
	std::vector<sTriangle> vecTriangles;
	std::vector<sMeshTriangle> vecMeshTriangles;

};

#endif 
