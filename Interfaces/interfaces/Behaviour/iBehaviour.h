#pragma once

#include <interfaces/iComponent.h>
#include <iObject.h>

class iBehaviour : public iComponent
{
	friend class cBehaviourManager;
public:
	virtual ~iBehaviour();
	virtual void start() = 0;
	virtual void update(float dt) = 0;

	virtual void OnCollide(iObject* other) {}
	virtual void OnDestroy() {}

	virtual bool serialize(rapidxml::xml_node<>* root_node) { return false; }
	virtual bool deserialize(rapidxml::xml_node<>* root_node) { return false; }

	sTransform& transform;
	iObject& parent;

protected:
	iBehaviour(iObject* root, ComponentType type);
	iBehaviour(iObject* root);

private:
	bool _initialized;
	iBehaviour() = delete;
};
