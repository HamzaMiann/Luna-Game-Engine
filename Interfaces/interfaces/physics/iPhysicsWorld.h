#pragma once
#include <glm/glm_common.h>
#include <vector>
#include "iPhysicsComponent.h"
#include "iPhysicsDebugRenderer.h"

namespace nPhysics
{
	struct RayCastResult
	{
		vec3 hitPoint;
		vec3 hitNormal;
		iPhysicsComponent* object;
	};

	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {}
		
		virtual void Update(float dt) = 0;

		virtual bool AddComponent(iPhysicsComponent* component) = 0;
		virtual bool RemoveComponent(iPhysicsComponent* component) = 0;

		/*
			@params
				- ro:	ray origin
				- rd:	normalized ray direction
				- t:	maximum distance of ray
			@returns
				- vector of iPhysicsComponents that were hit by the ray
		*/
		virtual std::vector<RayCastResult> RayCastAll(vec3 ro, vec3 rd, float t) = 0;

		/*
			@params
				- ro:	ray origin
				- rd:	normalized ray direction
				- t:	maximum distance of ray
			@returns
				- iPhysicsComponent of the first collider that was hit
		*/
		virtual RayCastResult* RayCast(vec3 ro, vec3 rd, float t) = 0;

		/*
			@params
				- renderer: the debug rendering instance
			@returns
				nothing
		*/
		virtual void SetDebugRenderer(iPhysicsDebugRenderer* renderer) = 0;


		inline void ToggleDebugMode() { mDebugMode = !mDebugMode; }
		inline bool IsDebugMode() { return mDebugMode; }

	protected:
		bool mDebugMode = false;

	};
}