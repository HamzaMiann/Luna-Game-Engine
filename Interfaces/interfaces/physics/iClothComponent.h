#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sClothDef
	{
		vec3 CornerA;
		vec3 CornerB;
		vec3 DownDirection;
		size_t NumNodesAcross;
		size_t NumNodesDown;
		float NodeMass;
		float NodeRadius;
		float springConstant;
		float PercentOfGravityApplied;
		vec3 windForce;
	};

	class iClothComponent : public iPhysicsComponent
	{
	public:
		virtual ~iClothComponent() {}

		virtual size_t NumNodes() = 0;
		virtual bool GetNodeRadius(size_t index, float& radiusOut) = 0;
		virtual bool GetNodePosition(size_t index, vec3& positionOut) = 0;
		virtual size_t NumSprings() = 0;
		virtual bool GetSpring(size_t index, std::pair<size_t, size_t>& springOut) = 0;

		/*

		SERIALIZATION

		*/
		virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; };
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; };

		/*

		FORCE

		*/
		virtual void AddForce(const vec3& force) override {}

		/*

		VELOCITY

		*/
		virtual void AddVelocity(const vec3& velocity) override {}
		virtual void SetVelocity(const vec3& velocity) override {}
		virtual vec3 GetVelocity() override { return vec3(0.f); }

		/*

		POSITION

		*/
		virtual void SetPosition(const glm::vec3& position) override {}
		virtual vec3 GetPosition() override { return vec3(0.f); }

		/*

		ROTATION

		*/
		virtual void SetRotation(const quat& rotation) override {}
		virtual quat GetRotation() override { return quat(vec3(0.f)); }

		/*

		TRANSFORM

		*/
		virtual void UpdateTransform() override {}
		virtual void GetTransform(mat4& transformOut) override {}

	protected:
		iClothComponent(iObject* parent) : iPhysicsComponent(parent, eComponentType::cloth) {}
	private:
		iClothComponent(const iClothComponent& other) = delete;
		iClothComponent& operator=(const iClothComponent& other) = delete;
	};
}