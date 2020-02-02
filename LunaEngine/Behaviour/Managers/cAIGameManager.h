#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
namespace nPhysics
{
	class iPhysicsComponent;
}

class cAIGameManager : public iBehaviour
{
	typedef glm::vec3 vec3;
public:
	virtual ~cAIGameManager() {}

	cAIGameManager(iObject* root):
		iBehaviour(root)
	{ }

	// Inherited via iBehaviour
	virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }
	virtual void start() override;
	virtual void update(float dt) override;

	void Player_Shoot(glm::vec3 const& start, glm::vec3 const& velocity);
	void Enemy_Shoot(glm::vec3 const& start, glm::vec3 const& velocity);

	std::vector<nPhysics::iPhysicsComponent*> player_bullets;

	float time_passed;

	class cBullet : public iBehaviour
	{
	public:
		virtual ~cBullet() {}

		cBullet(iObject* root) :
			iBehaviour(root)
		{
		}

		virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }
		virtual void start() override;
		virtual void update(float dt) override;

		virtual void OnCollide(iObject* other) override;

		std::string attack_layer;
		float life_time = 3.f;
		float t = 0.f;

		cAIGameManager* manager;

	};

};