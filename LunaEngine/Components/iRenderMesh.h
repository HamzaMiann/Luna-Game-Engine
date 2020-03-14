#pragma once

#include <iObject.h>

class iRenderMesh : public iComponent
{
public:

	virtual ~iRenderMesh() {}

	virtual bool serialize(rapidxml::xml_node<>* root_node) { return false; }
	virtual bool deserialize(rapidxml::xml_node<>* root_node) = 0;

	void Render() {}
	void Render(std::string layer_name) { if (layer_name == layer) Render(); }

protected:
	std::string layer;
	sTransform& transform;
	iObject& parent;

	iRenderMesh(iObject& parent) :
		transform(parent.transform),
		parent(parent),
		layer("default")
	{ }

	virtual void _render() = 0;
};