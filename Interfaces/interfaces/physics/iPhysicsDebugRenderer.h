#pragma once
#include <glm/glm_common.h>

namespace nPhysics
{
	class iPhysicsDebugRenderer
	{
	public:
		virtual ~iPhysicsDebugRenderer() {}
		virtual void DrawSphere(const vec3& center, float radius, const vec3& colour) = 0;
		virtual void DrawSphere(const mat4& transform, float radius, const vec3& colour) = 0;
		virtual void DrawCube(const vec3& center, const vec3& scale, const vec3& colour) = 0;
		virtual void DrawLine(const vec3& from, const vec3& to, const vec3& colour) = 0;
		virtual void DrawTriangle(const vec3& a, const vec3& b, const vec3& c, const vec3& colour) = 0;
	};
}