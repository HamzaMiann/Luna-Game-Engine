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


	std::vector<GraphNode*> closedList;
	std::mutex mtx;

	std::vector<GraphNode*> Dijkstra(GraphNode* root, GraphNode* target, Graph& graph)
	{
		std::vector<GraphNode*> path;

		std::vector<GraphNode*> openList;
		openList.push_back(root);

		std::map<GraphNode*, TableRow> rows;
		for (auto& node : graph.nodes) {
			rows[node] = TableRow(node);
			node->visited = false;
		}

		rows[root].distance = 0.f;

		while (!openList.empty())
		{
			float dist = FLT_MAX;
			GraphNode* current = 0;
			for (auto& node : openList)
			{
				auto& row = rows[node];
				if (row.distance < dist)
				{
					dist = row.distance;
					current = node;
				}
			}

			if (current == target) break;

			current->visited = true;

			for (auto& edge : current->children)
			{
				GraphNode* childNode = edge->GetOther(current);
				vec3 dir = glm::normalize(childNode->position - current->position);
				vec3 targetDir = glm::normalize(target->position - current->position);
				if (!childNode->visited && glm::dot(targetDir, dir) >= 0.f)
				{
					rows[childNode].previous = current;
					rows[childNode].distance = dist + edge->GetDistanceToOther(current);
					openList.push_back(childNode);
				}
			}

			openList.erase(FIND(current, openList));
			mtx.lock();
			closedList.push_back(current);
			mtx.unlock();
		}

		TableRow& end = rows[target];
		while (end.previous != nullptr)
		{
			path.insert(path.begin(), end.vertex);
			end = rows[end.previous];
		}

		path.insert(path.begin(), end.vertex);

		mtx.lock();
		closedList.clear();
		mtx.unlock();

		return path;
	}

	void DijkstraThreaded(GraphNode* root, GraphNode* target, Graph* graph)
	{
		Dijkstra(root, target, *graph);
	}

	void cPathFindingManager::start()
	{
		sTextureData texture;
		if (!cBasicTextureManager::Instance()->LoadJPGFromFile("assets/resourceMap.bmp", texture))
		{
			printf("Couldn't load bmp\n");
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
				cGameObject* player = new cGameObject;
				player->shader = obj->shader;
				player->meshName = "sphere";
				player->texture[0].SetTexture("blue.png", 1.f);
				player->transform.Position(obj->transform.Position() + vec3(0.f, 2.f, 0.f));
				player->transform.scale.x = 0.3f;
				player->transform.scale.z = 0.3f;
				cEntityManager::Instance().AddEntity(player);
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

		//(new std::thread(DijkstraThreaded, playerNode, resources[1], graph))->detach();

		auto path = Dijkstra(playerNode, resources[1], *graph);

		for (int i = 0; i < path.size() - 1; ++i)
		{
			cDebugRenderer::Instance().addLine(path[i]->position, path[i+1]->position, vec3(1.f, 0.f, 0.f), 10000.f);
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
				if (!(edge->GetDistToA() > 100.f || edge->GetDistToB() > 100.f)) {
					cDebugRenderer::Instance().addLine(edge->A->position + vec3(0.f, 1.f, 0.f), edge->B->position + vec3(0.f, 1.f, 0.f), vec3(1.f, 1.f, 0.f), dt);
				}
			}
		}

		mtx.lock();
		for (int i = 0; i < closedList.size(); ++i)
		{
			cDebugRenderer::Instance().addLine(playerNode->position, closedList[i]->position, vec3(1.f, 0.f, 0.f), 10.f);
		}
		mtx.unlock();
	}

}
