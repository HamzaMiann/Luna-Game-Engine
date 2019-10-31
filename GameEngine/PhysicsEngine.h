#pragma once

class Scene;
class cGameObject;
#include "cDebugRenderer.h"
#include "cAABB.h"

class PhysicsEngine
{
public:
	PhysicsEngine() {}
	~PhysicsEngine() {}

	const glm::vec3 Gravity = glm::vec3(0.f, -9.8f, 0.f);
	const float drag = 0.8f;
	const float friction = 0.7f;

	std::map<cGameObject*, cAABB> AABBs;

	cDebugRenderer* renderer;

	void IntegrationStep(Scene* scene, float deltaTime);
	void CheckCollisions(Scene* scene);
	void GenerateAABB(Scene* scene) {}
};

void FindClosestPointToMesh(Scene& scene, float& closestDistanceSoFar, glm::vec3& closestPoint, glm::vec3& normalVector, cGameObject* const meshObject, cGameObject* const pObj);