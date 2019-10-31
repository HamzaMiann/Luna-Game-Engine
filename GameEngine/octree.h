#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "cAABB.h"
#include "cMesh.h"


#define DEPTH 7

class octree
{
public:

	struct octree_node
	{
		cAABB* AABB;
		octree_node* nodes[8];
		bool has_nodes = true;
		bool has_triangles = false;

		std::vector<sMeshTriangle*> triangles;

	}*main_node = 0;

	octree() {}
	~octree() {}

	void generate_tree(glm::vec3 min, float length);
	void attach_triangles(std::vector<sMeshTriangle> const& triangles);

private:
	octree_node* _generate(int depth, glm::vec3 min, float length);
	bool _attach(std::vector<sMeshTriangle> const& triangles);
};


