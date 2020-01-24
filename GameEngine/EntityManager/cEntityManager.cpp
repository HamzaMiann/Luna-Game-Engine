#include "cEntityManager.h"

cEntityManager::cEntityManager()
{
}

cEntityManager::~cEntityManager()
{
	for (size_t i = 0; i < Entities.size(); ++i)
	{
		delete Entities[i];
	}
	Entities.clear();
}

bool cEntityManager::AddEntity(iObject* entity)
{
	auto it = std::find(Entities.begin(), Entities.end(), entity);
	if (it == Entities.end())
	{
		Entities.push_back(entity);
		return true;
	}
	return false;
}

bool cEntityManager::RemoveEntity(iObject* entity)
{
	auto it = std::find(Entities.begin(), Entities.end(), entity);
	if (it != Entities.end())
	{
		Entities.erase(it);
		return true;
	}
	return false;
}
