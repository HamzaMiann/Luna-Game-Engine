#pragma once


#include "../nConvert.h"
#include <interfaces/physics/iPlaneComponent.h>
#include "cBulletWrapperComponent.h"

namespace nPhysics
{
	class cPlaneComponent : public iPlaneComponent, public cBulletWrapperComponent
	{
	public:
		cPlaneComponent(iObject* parent, const sPlaneDef& def);
		virtual ~cPlaneComponent();

		void GetTransform(glm::mat4& transformOut);

		// Inherited via iPlaneComponent
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;
		virtual void AddForce(const glm::vec3& force) override;
		virtual void SetVelocity(const glm::vec3& velocity) override;
		virtual glm::vec3 GetVelocity() override;
		virtual void UpdateTransform() override;

		// Inherited via iPlaneComponent
		virtual void AddVelocity(const glm::vec3& velocity) override;
		virtual void SetPosition(const glm::vec3& position) override;
		virtual glm::vec3 GetPosition() override;
	};
}