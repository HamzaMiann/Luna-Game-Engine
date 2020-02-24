#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <Behaviour/AI/cFormationBehaviour.h>

class cFormationGameManager : public iBehaviour
{
	typedef glm::vec3 vec3;
public:
	virtual ~cFormationGameManager() {}

	cFormationGameManager(iObject* root) :
		iBehaviour(root)
	{
	}

	enum class FORM
	{
		Circle,
		V,
		Square,
		Line,
		TwoRows
	} formation = FORM::Circle;

	std::vector<AI::cFormationBehaviour*> agents;
	std::vector<vec3> positions;

	AI::AI_STATE state;

	virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }

	virtual void start() override;
	virtual void update(float dt) override;

private:

	float maxVelocity;

	void HandleInput();
	void HandleState();


	void NotifyStateChange();

	void SetCircle();
	void SetV();
	void SetSquare();
	void SetLine();
	void SetRows();

};