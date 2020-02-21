#pragma once

#include "../nConvert.h"
#include "cBulletWrapperComponent.h"
#include <interfaces/physics/iCubeComponent.h>

namespace nPhysics
{
	class cCubeComponent : public iCubeComponent, public cBulletWrapperComponent
	{
	public:
		cCubeComponent(iObject* parent, const sCubeDef& def);
		virtual ~cCubeComponent();

		// Inherited via iCubeComponent
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;
		virtual void GetTransform(glm::mat4& transformOut) override;
		virtual void AddForce(const glm::vec3& force) override;
		virtual void SetVelocity(const glm::vec3& velocity) override;
		virtual glm::vec3 GetVelocity() override;
		virtual void AddVelocity(const glm::vec3& velocity) override;
		virtual void SetPosition(const glm::vec3& position) override;
		virtual glm::vec3 GetPosition() override;
		virtual void UpdateTransform() override;

		vec3 offset;
	};
}