#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <Behaviour/AI/cFormationBehaviour.h>
class cGameObject;

class cFormationGameManager : public iBehaviour
{
	typedef glm::vec3 vec3;
public:
	virtual ~cFormationGameManager() {}

	cFormationGameManager(iObject* root) :
		iBehaviour(root)
	{
	}

	float separationRadius;
	float alignmentRadius;
	float cohesionRadius;

	struct Weights
	{
		float separation;
		float alignment;
		float cohesion;
	} weight;

	enum class FORM
	{
		Circle,
		V,
		Square,
		Line,
		TwoRows
	} formation = FORM::Circle;

	int nodeIndex = 0;
	int nodeStep = 1;

	std::vector<AI::cFormationBehaviour*> agents;
	std::vector<vec3> positions;
	std::vector<cGameObject*> nodes;

	AI::AI_STATE state;

	virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }

	virtual void start() override;
	virtual void update(float dt) override;

private:

	float maxVelocity;
	vec3 velocity;

	bool screenLocked;

	void IncreaseWeight(float& increased, float& decreased1, float& decreased2, float dt);
	void DecreaseWeight(float& decreased, float& increased1, float& increased2, float dt);

	void HandleInput(float dt);
	void HandleState(float dt);

	void DisplayWeights();

	void NotifyStateChange(AI::AI_STATE _state);

	void SetCircle();
	void SetV();
	void SetSquare();
	void SetLine();
	void SetRows();

};