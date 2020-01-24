#pragma once

#include <vector>
#include <iObject.h>

class cEntityManager
{
private:
	std::vector<iObject*> Entities;
	cEntityManager();
public:

	~cEntityManager();

	static cEntityManager* Instance()
	{
		static cEntityManager instance;
		return &instance;
	}

	inline const std::vector<iObject*>& GetEntities()
	{
		return Entities;
	}

	bool AddEntity(iObject* entity);

	bool RemoveEntity(iObject* entity);

};