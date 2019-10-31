

#include "octree.h"

octree::octree_node* octree::_generate(int depth, glm::vec3 min, float length)
{
	if (depth > DEPTH)
		return nullptr;

	octree::octree_node* node = new octree::octree_node;

	node->AABB = new cAABB;
	node->AABB->min = min;
	node->AABB->length = length;

	if (depth == DEPTH) node->has_nodes = false;

	float half = length / 2.f;

	node->nodes[0] = _generate(depth + 1,
							   min,
							   length / 2.f);

	node->nodes[1] = _generate(depth + 1,
							   min + glm::vec3(half, 0.f, 0.f),
							   length / 2.f);

	node->nodes[2] = _generate(depth + 1,
							   min + glm::vec3(0.f, half, 0.f),
							   length / 2.f);

	node->nodes[3] = _generate(depth + 1,
							   min + glm::vec3(half, half, 0.f),
							   length / 2.f);

	node->nodes[4] = _generate(depth + 1,
							   min + glm::vec3(0.f, 0.f, half),
							   length / 2.f);

	node->nodes[5] = _generate(depth + 1,
							   min + glm::vec3(0.f, half, half),
							   length / 2.f);

	node->nodes[6] = _generate(depth + 1,
							   min + glm::vec3(half, 0.f, half),
							   length / 2.f);

	node->nodes[7] = _generate(depth + 1,
							   min + glm::vec3(half, half, half),
							   length / 2.f);

	return node;
}

void octree::generate_tree(glm::vec3 min, float length)
{
	main_node = _generate(1, min, length);
}

void octree::attach_triangles(std::vector<sMeshTriangle> const& triangles)
{
	for (unsigned int i = 0; i < triangles.size(); ++i)
	{
		
	}
}
