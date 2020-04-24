#pragma once


#include "iSceneBuilder.h"

struct ModelLoadInfo
{
	ModelLoadInfo() :
		fileName(""),
		friendlyName(""),
		next(0)
	{
	}
	~ModelLoadInfo()
	{
		if (next)
		{
			delete next;
			next = 0;
		}
	}

	std::string fileName;
	std::string friendlyName;
	int MAX_MESHES;
	ModelLoadInfo* next;
};

class cModelBuilder : public iSceneBuilder
{
public:
	cModelBuilder() {}
	virtual ~cModelBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};