#ifndef _cModelLoader_HG_2019_09_09_
#define _cModelLoader_HG_2019_09_09_

#include <string>	
#include <vector>
#include <Mesh/cMesh.h>

struct LoadResult
{
	bool success = false;
	std::vector<std::string> textures;
};

class cModelLoader
{
private:
	cModelLoader();			// constructor

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

	bool LoadAnimation(	std::string filename,
						std::string friendlyName,
						std::string meshName);

};

#endif //_cModelLoader_HG_2019_09_09_

