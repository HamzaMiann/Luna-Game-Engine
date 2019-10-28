#pragma once

#include <glm/vec3.hpp>

template <typename T>
class octree
{
private:

	struct octree_node
	{
		glm::vec3 minPos;
		float length;
		octree_node* nodes[8];
	} main_node;

public:
	octree() {}
	~octree() {}
};