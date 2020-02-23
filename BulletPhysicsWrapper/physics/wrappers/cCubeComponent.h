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

		/*
		
		SERIALIZATION

		*/
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

		/*

		FORCE

		*/
		virtual void AddForce(const vec3& force) override;

		/*

		VELOCITY

		*/
		virtual void AddVelocity(const vec3& velocity) override;
		virtual void SetVelocity(const vec3& velocity) override;
		virtual vec3 GetVelocity() override;

		/*

		POSITION

		*/
		virtual void SetPosition(const glm::vec3& position) override;
		virtual vec3 GetPosition() override;

		/*

		ROTATION

		*/
		virtual void SetRotation(const quat& rotation) override;
		virtual quat GetRotation() override;

		/*

		TRANSFORM

		*/
		virtual void UpdateTransform() override;
		virtual void GetTransform(mat4& transformOut) override;

		
	};
}