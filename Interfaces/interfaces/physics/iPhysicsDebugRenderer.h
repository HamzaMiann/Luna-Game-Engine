#pragma once
#include <glm/glm_common.h>

namespace nPhysics
{
	class iPhysicsDebugRenderer
	{
	public:
		virtual ~iPhysicsDebugRenderer() {}
		virtual void DrawSphere(const vec3& center, float radius) = 0;
		virtual void DrawCube(const vec3& center, const vec3& scale) = 0;
		virtual void DrawLine(const vec3& from, const vec3& to) = 0;
		virtual void DrawTriangle(const vec3& a, const vec3& b, const vec3& c) = 0;
	};
}