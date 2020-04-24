

#include "cModelBuilder.h"
#include <Mesh/cVAOManager.h>
#include <Mesh/cModelLoader.h>
#include <Shader/Shader.h>
#include <iostream>
#include <Threading/threading.h>
using namespace rapidxml;

//#define THREADED

void LoadModel(ModelLoadInfo* info)
{
	cModelLoader& ModelLoader = cModelLoader::Instance();

	cMesh* pMesh = new cMesh();
	if (ModelLoader.LoadModel("assets/models/" + info->fileName, info->friendlyName, *pMesh, info->MAX_MESHES).success)
	{
#ifdef THREADED
		Thread::Dispatch([info, pMesh]()
			{
				sModelDrawInfo* pDrawInfo = new sModelDrawInfo();
				cVAOManager::Instance()
					.LoadModelIntoVAO(
						info->friendlyName,
						*pMesh,
						*pDrawInfo,
						Shader::FromName("basic")->GetID()
					);
			}
		);
#else
		sModelDrawInfo* pDrawInfo = new sModelDrawInfo();
		cVAOManager::Instance()
			.LoadModelIntoVAO(
				info->friendlyName,
				*pMesh,
				*pDrawInfo,
				Shader::FromName("basic")->GetID()
			);
#endif
	}

	if (info->next != nullptr)
	{
		LoadModel(info->next);
	}
}

void cModelBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Loading models...\n");

	

	ModelLoadInfo* first = 0;
	ModelLoadInfo* previous = 0;

	for (xml_node<>* model_node = node->first_node("Model"); model_node; model_node = model_node->next_sibling("Model"))
	{
		xml_attribute<>* file = model_node->first_attribute("file");
		xml_attribute<>* friendly = model_node->first_attribute("friendlyName");
		xml_attribute<>* shader = model_node->first_attribute("shader");
		xml_attribute<>* meshes = model_node->first_attribute("meshes");
		if (file)
		{
			std::string fileName = file->value();
			std::string friendlyName = friendly->value();

			ModelLoadInfo* info = new ModelLoadInfo;
			info->friendlyName = friendlyName;
			info->fileName = fileName;
			info->MAX_MESHES = INT_MAX;
			if (meshes)
			{
				info->MAX_MESHES = std::atoi(meshes->value());
			}
			if (previous)
			{
				previous->next = info;
				previous = info;
			}
			else
			{
				first = info;
				previous = first;
			}
		}

	}

	if (first)
	{
#ifdef THREADED
		(new std::thread(LoadModel, first))->detach();
#else
		LoadModel(first);
		delete first; first = 0;
#endif
	}
}