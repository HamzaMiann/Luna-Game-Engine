#pragma once

#include <interfaces/physics/iSphereComponent.h>
#include <interfaces/physics/iPlaneComponent.h>
#include <luna_physics/cRigidBody.h>
#include <glm/mat4x4.hpp>

class cSphereComponent : public nPhysics::iSphereComponent, public phys::cRigidBody
{
public:
	// Inherited via iSphereComponent
	virtual void GetTransform(glm::mat4& transformOut) override;
	virtual bool serialize(rapidxml::xml_node<>* root_node) override;
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;
};

class cPlaneComponent : public nPhysics::iPlaneComponent, public phys::cRigidBody
{
public:
	// Inherited via iPlaneComponent
	virtual void GetTransform(glm::mat4& transformOut) override;
	virtual bool serialize(rapidxml::xml_node<>* root_node) override;
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;
};