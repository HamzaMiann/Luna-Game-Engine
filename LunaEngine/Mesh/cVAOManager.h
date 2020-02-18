#ifndef _cVAOManager_HG_
#define _cVAOManager_HG_

#include <_GL/GLCommon.h>

// Will load the models and place them 
// into the vertex and index buffers to be drawn

#include <string>
#include <map>
#include <Mesh/cMesh.h>				// File as loaded from the file

// The vertex structure 
//	that's ON THE GPU (eventually) 
// So dictated from THE SHADER
struct sVAOVertex
{
	sVAOVertex() :
		x(0.f), y(0.f), z(0.f), w(1.f),
		nx(0.f), ny(0.f), nz(0.f), nw(1.f),
		u0(0.f), v0(0.f), u1(0.f), v1(0.f),
		tx(0.f), ty(0.f), tz(0.f), tw(1.f),
		bx(0.f), by(0.f), bz(0.f), bw(1.f)
	{
		memset(this->boneID, 0, sizeof(unsigned int) * NUMBEROFBONES);
		memset(this->weights, 0, sizeof(float) * NUMBEROFBONES);
	}
	float x, y, z, w;
	float nx, ny, nz, nw;
	float u0, v0, u1, v1;
	float tx, ty, tz, tw;
	float bx, by, bz, bw;
	float boneID[4];
	float weights[4];
};


struct sModelDrawInfo
{
	sModelDrawInfo(); 

	std::string meshName;

	unsigned int VAO_ID;

	unsigned int VertexBufferID;
	unsigned int VertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int IndexBufferID;
	unsigned int IndexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The "local" (i.e. "CPU side" temporary array)
	sVAOVertex* pVertices;	//  = 0;
	// The index buffer (CPU side)
	unsigned int* pIndices;		
};


class cVAOManager
{
public:

	static cVAOManager& Instance()
	{
		static cVAOManager instance;
		return instance;
	}

	// Takes a cMesh object and copies it into the GPU (as a VOA)
	bool LoadModelIntoVAO(std::string fileName, 
						  cMesh &theMesh,				// NEW
						  sModelDrawInfo &drawInfo, 
						  unsigned int shaderProgramID);

	// We don't want to return an int, likely
	bool FindDrawInfoByModelName(std::string filename,
								 sModelDrawInfo &drawInfo);

	cMesh* FindMeshByModelName(std::string filename);

	std::string getLastError(bool bAndClear = true);

private:

	cVAOManager() {}

	std::map< std::string /*model name*/,
		      sModelDrawInfo /* info needed to draw*/ >
		m_map_ModelName_to_VAOID;

	std::map<	std::string /*model name*/,
				cMesh* /*reference to the drawn mesh for use in physics calculations*/
			>
		m_map_ModelName_to_cMesh;
};

#endif	// _cVAOManager_HG_
