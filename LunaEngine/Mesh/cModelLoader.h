#ifndef _cModelLoader_HG_2019_09_09_
#define _cModelLoader_HG_2019_09_09_

#include <string>	
#include <vector>
#include <Mesh/cMesh.h>

struct aiScene;
struct aiMesh;

struct LoadResult
{
	bool success = false;
	std::vector<std::string> textures;
};

class cModelLoader
{
private:
	cModelLoader();			// constructor

	void LoadMeshes(const aiScene* scene, cMesh& theMesh);
	void LoadTextures(const aiScene* scene, cMesh& theMesh, LoadResult& result);
	void LoadBones(const aiScene* scene, aiMesh& mesh, cMesh& theMesh);
	void LoadAnimations(const aiScene* scene, cMesh& theMesh);

public:

	static cModelLoader& Instance()
	{
		static cModelLoader instance;
		return instance;
	}

	~cModelLoader();		// destructor

	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(FLT_MIN);

	// Takes the filename to load
	// Returns by ref the mesh
	LoadResult LoadModel(	std::string filename,
							std::string friendlyName,
							cMesh &theMesh);		// Note the & (by reference)


};

#endif //_cModelLoader_HG_2019_09_09_

