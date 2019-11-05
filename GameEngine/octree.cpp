

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
	this->_attach(triangles, main_node);
}

int i = 0;
bool octree::_attach(std::vector<sMeshTriangle> const& triangles, octree::octree_node* node)
{
	if (node->has_nodes)
	{
		bool hasTriangles = false;
		for (int i = 0; i < 8; ++i)
		{
			hasTriangles |= _attach(triangles, node->nodes[i]);
		}
		node->has_triangles = hasTriangles;
		return hasTriangles;
	}
	else
	{
		printf("%d node\n", i++);
		bool hasTriangles = false;
		for (int i = 0; i < triangles.size(); ++i)
		{
			if (node->AABB->contains(triangles[i].first))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].second))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].third))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
		}
		node->has_triangles = hasTriangles;
		return hasTriangles;
	}
}

octree::octree_node::~octree_node()
{
	for (int i = 0; i < 8; ++i)
	{
		if (this->nodes[i])
		{
			delete this->nodes[i];
			this->nodes[i] = nullptr;
			this->has_nodes = false;
		}
	}
}


octree::octree_node* octree::find_node(glm::vec3 const& point)
{
	return _find(point, main_node);
}

octree::octree_node* octree::_find(glm::vec3 const& point, octree_node* node)
{
	if (!node->has_nodes) return node;
	if (node->AABB->contains(point))
	{
		for (int i = 0; i < 8; ++i)
		{
			if (node->nodes[i]->AABB->contains(point))
			{
				return _find(point, node->nodes[i]);
			}
		}
		return node;
	}
	return nullptr;
}