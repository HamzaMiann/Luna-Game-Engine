
#include "cGameObject.h"

cGameObject::~cGameObject()
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		delete components[i];
	}
}

void cGameObject::Initialize()
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		components[i]->init();
	}
}

void cGameObject::Update()
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		components[i]->update();
	}
}

void cGameObject::Collide(cGameObject* other)
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		components[i]->collide(other);
	}
}