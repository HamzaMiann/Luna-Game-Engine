
#include "Scene.h"
#include "cSceneFactory.h"
#include <fstream>
#include <rapidxml/rapidxml.hpp>
#include <vector>
#include <iostream>
using namespace rapidxml;
using namespace std;

Scene::~Scene()
{
	if (this->pAudioEngine)
		delete pAudioEngine;
	if (this->pModelLoader)
		delete pModelLoader;
	if (this->pVAOManager)
		delete pVAOManager;
}

Scene* Scene::LoadFromXML(std::string filename)
{
	Scene* newScene = new Scene();
	xml_document<> doc;
	xml_node<>* root_node;
	cSceneFactory factory;

	ifstream filestream("assets/scenes/" + filename);
	if (!filestream.is_open())
	{
		cout << "Scene could not find/open file: " << filename << endl;
		exit(1);
	}

	vector<char> buffer((istreambuf_iterator<char>)(filestream), istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Scene");

	for (xml_node<>* node = root_node->first_node(); node; node = node->next_sibling())
	{
		iSceneBuilder* builder = factory.GetBuilder(node->name());
		if (builder)
		{
			builder->Build(*newScene, node);
			delete builder;
		}
	}

	return newScene;

}