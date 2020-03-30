#pragma once
#include <Texture/cBasicTextureManager.h>
#include <vector>
#include <glm/glm_common.h>

class GraphEdge;
struct GraphNode
{
	vec3 position = vec3(0.f);
	float weight = 10.f;
	char tag = 0;
	std::vector<GraphEdge*> children;
	bool visited = false;
	float gCostSoFar = FLT_MAX;
	float hDist = FLT_MAX;
	inline bool isTraversable() { return weight < 100.f; }
};

struct GraphEdge
{
public:
	GraphEdge():
		A(0),
		B(0),
		weightFactor(1.f)
	{}
	GraphNode* A;
	GraphNode* B;
	float weightFactor;

	inline float GetDistToA() { return A->weight * weightFactor; }
	inline float GetDistToB() { return B->weight * weightFactor; }
	inline float GetDistanceToOther(GraphNode* node) { return A == node ? GetDistToB() : GetDistToA(); }
	inline GraphNode* GetOther(GraphNode* node) { return A == node ? B : A; }
};

class Graph
{
	char GetColourCharacter(unsigned char r, unsigned char g, unsigned char b);
public:

	Graph(sTextureData& source);
	~Graph();

	std::vector<GraphNode*> nodes;
	std::vector<GraphEdge*> edges;
};