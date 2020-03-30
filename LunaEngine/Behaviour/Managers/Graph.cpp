#include "Graph.h"

char Graph::GetColourCharacter(unsigned char r, unsigned char g, unsigned char b)
{
	if (r == 255u && g == 0u && b == 0u)		return 'r';
	if (r == 0u && g == 255u && b == 0u)		return 'g';
	if (r == 0u && g == 0u && b == 255u)		return 'b';
	if (r == 255u && g == 255u && b == 255u)	return 'w';
	if (r == 0u && g == 0u && b == 0u)			return '_';
	return 'x';
}

Graph::Graph(sTextureData& source)
{
	vec3 start(-(float)source.width / 2.f, 0.f, -(float)source.height / 2.f);
	unsigned int index = 0;
	for (unsigned long x = 0.f; x < source.height; ++x)
	{
		for (unsigned long y = 0; y < source.width; ++y)
		{
			vec3 pos(0.f);
			pos.x = (start.x + x * 2.f);
			pos.z = (start.z + y * 2.f);
			GraphNode* node = new GraphNode;
			node->position = pos;

			unsigned char r, g, b;
			r = source.data[index++];
			g = source.data[index++];
			b = source.data[index++];
			char c = GetColourCharacter(r, g, b);

			node->tag = c;
			
			if (c == '_') node->weight = 10000.f;
			if (c == 'x') node->weight = 20.f;

			nodes.push_back(node);
		}
	}

	for (unsigned long x = 0.f; x < source.height - 1; ++x)
	{
		for (unsigned long y = 0; y < source.width - 1; ++y)
		{
			unsigned int index1 = x * source.width + y;
			unsigned int index2 = index1 + 1;
			GraphEdge* edge = new GraphEdge;
			edge->A = nodes[index1];
			edge->B = nodes[index2];
			
			if (nodes[index1]->isTraversable() && nodes[index2]->isTraversable()) {
				nodes[index1]->children.push_back(edge);
				nodes[index2]->children.push_back(edge);
				edges.push_back(edge);
			}

			index2 = (x + 1) * source.width + y;
			edge = new GraphEdge;
			edge->A = nodes[index1];
			edge->B = nodes[index2];
			if (nodes[index1]->isTraversable() && nodes[index2]->isTraversable()) {
				nodes[index1]->children.push_back(edge);
				nodes[index2]->children.push_back(edge);
				edges.push_back(edge);
			}

			index2++;
			edge = new GraphEdge;
			edge->weightFactor = 1.4f;
			edge->A = nodes[index1];
			edge->B = nodes[index2];
			if (nodes[index1]->isTraversable() && nodes[index2]->isTraversable()) {
				nodes[index1]->children.push_back(edge);
				nodes[index2]->children.push_back(edge);
				edges.push_back(edge);
			}
		}
	}

	for (unsigned long x = 0.f; x < source.height - 1; ++x)
	{
		unsigned long y = source.height - 1;
		unsigned int index1 = x * source.width + y;
		unsigned int index2 = (x + 1) * source.width + y;
		GraphEdge* edge = new GraphEdge;
		edge->A = nodes[index1];
		edge->B = nodes[index2];
		if (nodes[index1]->isTraversable() && nodes[index2]->isTraversable()) {
			nodes[index1]->children.push_back(edge);
			nodes[index2]->children.push_back(edge);
			edges.push_back(edge);
		}
	}

	for (unsigned long y = 0.f; y < source.width - 1; ++y)
	{
		unsigned long x = source.height - 1;
		unsigned int index1 = x * source.width + y;
		unsigned int index2 = index1 + 1;
		GraphEdge* edge = new GraphEdge;
		edge->A = nodes[index1];
		edge->B = nodes[index2];
		if (nodes[index1]->isTraversable() && nodes[index2]->isTraversable()) {
			nodes[index1]->children.push_back(edge);
			nodes[index2]->children.push_back(edge);
			edges.push_back(edge);
		}
	}

	for (unsigned long x = 0.f; x < source.height - 1; ++x)
	{
		for (unsigned long y = source.width - 1; y > 0; --y)
		{
			unsigned int index1 = x * source.width + y;
			unsigned int index2 = (x + 1) * source.width + y - 1;

			GraphEdge* edge = new GraphEdge;
			edge->weightFactor = 1.4f;
			edge->A = nodes[index1];
			edge->B = nodes[index2];
			if (nodes[index1]->isTraversable() && nodes[index2]->isTraversable()) {
				nodes[index1]->children.push_back(edge);
				nodes[index2]->children.push_back(edge);
				edges.push_back(edge);
			}
		}
	}
}

Graph::~Graph()
{
	for (GraphEdge*& edge : edges)
	{
		delete edge;
		edge = 0;
	}
	edges.clear();

	for (GraphNode*& node : nodes)
	{
		delete node;
		node = 0;
	}
	nodes.clear();

	
}
