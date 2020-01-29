#pragma once

#include <glm/vec3.hpp>

namespace nCollide
{

	int intersect_moving_sphere_plane(const glm::vec3& c,
									  float r,
									  const glm::vec3& v,
									  const glm::vec3& n,
									  float d,
									  float& t,
									  glm::vec3& q);

	int intersect_moving_sphere_sphere(const glm::vec3& c0,
									   float r0,
									   const glm::vec3& v0,
									   const glm::vec3& c1,
									   float r1,
									   const glm::vec3& v1,
									   float& t);


	glm::vec3 closest_point_on_plane(const glm::vec3& point,
									 const glm::vec3& n,
									 float d);

}