#include <Behaviour/AI/cPathFollowingBehaviour.h>
#include "cPathFindingManager.h"
#include <iostream>
#include <cGameObject.h>
#include <Shader/Shader.h>
#include <EntityManager/cEntityManager.h>
#include <DebugRenderer/cDebugRenderer.h>
#include <InputManager.h>
#include <thread>
#include <mutex>

#define FIND(element, elements) std::find(elements.begin(), elements.end(), element)

namespace AI {

	struct TableRow
	{
		TableRow() {}
		TableRow(GraphNode* node):
			vertex(node)
		{}
		GraphNode* vertex;
		float distance = FLT_MAX;
		GraphNode* previous = nullptr;
	};

	std::vector<GraphNode*> AStar(GraphNode* root, GraphNode* target, Graph& graph)
	{
		std::vector<GraphNode*> path;
		std::vector<GraphNode*> openList;
		std::vector<GraphNode*> closedList;
		openList.push_back(root);

		std::map<GraphNode*, TableRow*> rows;
		for (auto& node : graph.nodes) {
			if (node->isTraversable())
				rows[node] = new TableRow(node);
			node->visited = false;
			node->gCostSoFar = FLT_MAX;
			node->hDist = glm::distance(node->position, target->position);
		}

		rows[root]->distance = 0.f;
		root->gCostSoFar = 0.f;

		while (!openList.empty())
		{
			float dist = FLT_MAX;
			GraphNode* current = 0;
			auto currentIT = openList.begin();
			for (auto it = openList.begin(); it != openList.end(); it++)
			{
				auto node = *it;
				float fCost = node->gCostSoFar + node->hDist;
				if (fCost < dist)
				{
					dist = fCost;
					current = node;
					currentIT = it;
				}
			}

			if (current == target)
			{
				break;
			}

			current->visited = true;
			openList.erase(currentIT);

			for (auto& edge : current->children)
			{
				GraphNode* childNode = edge->GetOther(current);
				if (!childNode->visited)
				{
					if (current->gCostSoFar + edge->GetDistanceToOther(current) < childNode->gCostSoFar) 
					{
						rows[childNode]->previous = current;
						childNode->gCostSoFar = current->gCostSoFar + edge->GetDistanceToOther(current);
					}
					//rows[childNode]->distance = childNode->gCostSoFar;
					if (FIND(childNode, openList) == openList.end())
						openList.push_back(childNode);
				}
			}

			closedList.push_back(current);
		}

		TableRow* end = rows[target];
		while (end->previous != nullptr)
		{
			path.insert(path.begin(), end->vertex);
			end = rows[end->previous];
		}

		path.insert(path.begin(), end->vertex);

		closedList.clear();

		return path;
	}


	std::vector<GraphNode*> Dijkstra(GraphNode* root, Graph& graph)
	{
		std::vector<GraphNode*> path;
		std::vector<GraphNode*> openList;
		std::vector<GraphNode*> closedList;
		openList.push_back(root);

		std::map<GraphNode*, TableRow*> rows;
		for (auto& node : graph.nodes) {
			if (node->isTraversable())
				rows[node] = new TableRow(node);
			node->visited = false;
			node->gCostSoFar = FLT_MAX;
		}

		rows[root]->distance = 0.f;
		root->gCostSoFar = 0.f;

		GraphNode* target = 0;

		while (!openList.empty())
		{
			float dist = FLT_MAX;
			GraphNode* current = 0;
			auto currentIT = openList.begin();
			for (auto it = openList.begin(); it != openList.end(); it++)
			{
				auto node = *it;
				if (node->gCostSoFar < dist)
				{
					dist = node->gCostSoFar;
					current = node;
					currentIT = it;
				}
			}

			if (current->tag == 'r')
			{
				target = current;
				target->tag = 'w';
				break;
			}

			current->visited = true;
			openList.erase(currentIT);

			for (auto& edge : current->children)
			{
				GraphNode* childNode = edge->GetOther(current);
				if (!childNode->visited)
				{
					if (childNode->gCostSoFar > dist + edge->GetDistanceToOther(current)) {
						rows[childNode]->previous = current;
						childNode->gCostSoFar = dist + edge->GetDistanceToOther(current);
						//rows[childNode]->distance = childNode->gCostSoFar;
					}
					if (FIND(childNode, openList) == openList.end())
						openList.push_back(childNode);
				}
			}

			closedList.push_back(current);
		}

		if (target != nullptr)
		{
			TableRow* end = rows[target];
			while (end->previous != nullptr)
			{
				path.insert(path.begin(), end->vertex);
				end = rows[end->previous];
			}

			path.insert(path.begin(), end->vertex);
		}

		closedList.clear();

		return path;
	}

	void cPathFindingManager::start()
	{
		sTextureData texture;
		if (!cBasicTextureManager::Instance()->LoadBMPFromFile("assets/resourceMap.bmp", texture))
		{
			printf("Couldn't load bmp\n");
			return;
		}

		graph = new Graph(texture);

		for (auto& node : graph->nodes)
		{
			cGameObject* obj = new cGameObject;
			obj->shader = Shader::FromName("basic");
			obj->meshName = "cube";
			obj->texture[0].SetTexture("blue.png", 1.f);
			obj->transform.Position(node->position);
			if (node->tag == '_')
			{
				obj->transform.scale.y = 2.f;
				obj->texture[0].SetTexture("WATER_BUMP.png", 1.f);
			}
			if (node->tag == 'x')
			{
				obj->texture[0].SetTexture("Ground_Dirt_007_basecolor.png", 1.f);
			}
			if (node->tag == 'g')
			{
				obj->texture[0].SetTexture("lens_dust.jpg", 1.f);
				cGameObject* player = cEntityManager::Instance().GetGameObjectByTag("agent");
				player->transform.Position(obj->transform.Position() + vec3(0.f, 1.f, 0.f));
				player->AddComponent<cPathFollowingBehaviour>()->current = node;
				this->playerNode = node;
			}
			if (node->tag == 'r')
			{
				obj->texture[0].SetTexture("red.png", 1.f);
				resources.push_back(node);
			}
			if (node->tag == 'b')
			{
				obj->texture[0].SetTexture("Mutant_normal.jpg", 1.f);
				home = node;
			}
			cEntityManager::Instance().AddEntity(obj);
		}
		
	}

	void cPathFindingManager::update(float dt)
	{
		if (Input::KeyUp(GLFW_KEY_T))
		{
			shouldShowGraph = !shouldShowGraph;
		}

		if (shouldShowGraph)
		{
			for (auto& edge : graph->edges)
			{
				cDebugRenderer::Instance().addLine(edge->A->position + vec3(0.f, 1.f, 0.f), edge->B->position + vec3(0.f, 1.f, 0.f), vec3(1.f, 1.f, 0.f), dt);
			}
		}
	}

	std::vector<GraphNode*> cPathFindingManager::GetPathToClosestResource(GraphNode* source)
	{
		return Dijkstra(source, *graph);
	}

	std::vector<GraphNode*> cPathFindingManager::GetPathToHome(GraphNode* source)
	{
		return AStar(source, home, *graph);
	}

}
