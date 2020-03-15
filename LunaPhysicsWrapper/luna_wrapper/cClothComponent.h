#pragma once
#include <interfaces/physics/iClothComponent.h>
#include <luna_physics/cSoftBody.h>

namespace nPhysics
{

	class cClothComponent : public iClothComponent, public phys::cSoftBody
	{
	public:
		cClothComponent(iObject* parent, const sClothDef& def);
		virtual ~cClothComponent();

	private:
		static phys::sSoftBodyDef ClothToSoftBodyDef(const sClothDef& def);

		// Inherited via iClothComponent
		virtual size_t NumNodes() override;
		virtual bool GetNodeRadius(size_t index, float& radiusOut) override;
		virtual bool GetNodePosition(size_t index, vec3& positionOut) override;
		virtual size_t NumSprings() override;
		virtual bool GetSpring(size_t index, std::pair<size_t, size_t>& springOut) override;
	};

}