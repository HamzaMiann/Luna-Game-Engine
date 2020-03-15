#include "cClothComponent.h"
#include "cPhysicsFactory.h"

namespace nPhysics
{
	cClothComponent::cClothComponent(iObject* parent, const sClothDef& def):
		iClothComponent(parent),
		cSoftBody(ClothToSoftBodyDef(def))
	{
		cPhysicsFactory().GetWorld()->AddComponent(this);
	}

	cClothComponent::~cClothComponent()
	{
		cPhysicsFactory().GetWorld()->RemoveComponent(this);
	}

	
	size_t cClothComponent::NumNodes()
	{
		return size();
	}

	bool cClothComponent::GetNodeRadius(size_t index, float& radiusOut)
	{
		if (index >= NumNodes()) return false;
		NodeRadius(index, radiusOut);
	}

	bool cClothComponent::GetNodePosition(size_t index, vec3& positionOut)
	{
		if (index >= NumNodes()) return false;
		NodePosition(index, positionOut);
	}

	size_t cClothComponent::NumSprings()
	{
		return mSprings.size();
	}

	bool cClothComponent::GetSpring(size_t index, std::pair<size_t, size_t>& springOut)
	{
		if (index >= NumSprings()) return false;

		size_t index1;
		size_t index2;
		for (unsigned int i = 0; i < NumNodes(); ++i)
		{
			if (mNodes[i] == mSprings[index]->NodeA) index1 = i;
			if (mNodes[i] == mSprings[index]->NodeB) index2 = i;
		}
		
		springOut = std::make_pair(index1, index2);
	}

	phys::sSoftBodyDef cClothComponent::ClothToSoftBodyDef(const sClothDef& def)
	{
		phys::sSoftBodyDef physDef;
		physDef.SpringConstant = def.springConstant;
		physDef.PercentOfGravityApplied = def.PercentOfGravityApplied;
		physDef.windForce = def.windForce;

		vec3 directionAcross = (def.CornerB - def.CornerA) / (float)def.NumNodesAcross;
		vec3 directionDown = def.DownDirection;
		vec3 origin = def.CornerA;

		for (size_t y = 0; y < def.NumNodesDown; ++y)
		{
			vec3 dirY = (float)y * directionDown;
			for (size_t x = 0; x < def.NumNodesAcross; ++x)
			{
				phys::sSoftBodyNodeDef node;
				vec3 dirX = (float)x * directionAcross;
				vec3 pos = origin + dirX + dirY;
				node.Position = pos;
				node.Mass = def.NodeMass;
				if (y == 0) node.Mass = 0.f;
				node.Radius = 0.5f;
				physDef.Nodes.push_back(node);
			}
		}

		/*for (size_t y = 0; y < def.NumNodesDown - 1; ++y)
		{
			for (size_t x = 0; x < def.NumNodesAcross - 1; ++x)
			{
				size_t index1 = y * def.NumNodesAcross + x;
				size_t index2 = index1 + 1;
				physDef.Springs.push_back(std::make_pair(index1, index2));

				index2 = index1 + def.NumNodesAcross;
				physDef.Springs.push_back(std::make_pair(index1, index2));
			}
		}*/


		for (size_t y = 0; y < def.NumNodesDown; ++y)
		{
			for (size_t x = 0; x < def.NumNodesAcross - 1; ++x)
			{
				size_t index1 = y * def.NumNodesAcross + x;
				size_t index2 = index1 + 1;
				physDef.Springs.push_back(std::make_pair(index1, index2));
			}
		}

		for (size_t y = 0; y < def.NumNodesDown - 1; ++y)
		{
			for (size_t x = 0; x < def.NumNodesAcross; ++x)
			{
				size_t index1 = y * def.NumNodesAcross + x;
				size_t index2 = index1 + def.NumNodesAcross;
				physDef.Springs.push_back(std::make_pair(index1, index2));
			}
		}

		for (auto i : physDef.Springs)
		{
			printf("{ %d, %d }\n", i.first, i.second);
		}

		return physDef;
	}

}