#pragma once

#include <iSerializable.h>

class cRigidBody;
class cMeshRenderer;
class cMesh;

class iComponent : public iSerializable
{
public:
	
	iComponent()
	{
		rigidBody		= 0;
		mesh			= 0;
		meshRenderer	= 0;
	}
	virtual ~iComponent() {}

	virtual bool serialize(rapidxml::xml_node<>* root_node) = 0;
	virtual bool deserialize(rapidxml::xml_node<>* root_node) = 0;
	
	cRigidBody*		rigidBody;
	cMesh*			mesh;
	cMeshRenderer*	meshRenderer;
};