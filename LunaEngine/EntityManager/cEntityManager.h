#pragma once

#include <vector>
#include <string>
class cGameObject;

class cEntityManager
{
private:
	std::vector<cGameObject*>* Entities;
	cEntityManager();
public:

	~cEntityManager();

	static cEntityManager* Instance()
	{
		static cEntityManager instance;
		return &instance;
	}

	inline const std::vector<cGameObject*> GetEntities()
	{
		return *Entities;
	}

	bool AddEntity(cGameObject* entity);

	bool RemoveEntity(cGameObject* entity);

	cGameObject* GetObjectByTag(std::string tag);

	void SetEntities(std::vector<cGameObject*>* entities);

	void Update(float dt);

};