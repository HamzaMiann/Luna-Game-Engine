#pragma once

#include <iObject.h>
#include <interfaces/physics/iSphereComponent.h>
#include <interfaces/physics/iPlaneComponent.h>
#include <luna_physics/shapes.h>
#include <glm/mat4x4.hpp>

class cSphereComponent : public nPhysics::iSphereComponent, public phys::cSphereBody
{
public:
	cSphereComponent(iObject* parent, const nPhysics::sSphereDef& definition);
	virtual ~cSphereComponent() {}


	// Inherited via iSphereComponent
	virtual const glm::vec3& GetPosition() override;
	virtual void SetPosition(const glm::vec3& pos) override;

	virtual const glm::vec3& GetVelocity() override;
	virtual void SetVelocity(const glm::vec3& vel) override;

	virtual void UpdateTransform() override;

	virtual void AddForce(const glm::vec3& force) override;

	virtual bool serialize(rapidxml::xml_node<>* root_node) override;

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

};