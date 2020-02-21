#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <glm/glm_common.h>

class cSimpleAssimpSkinnedMesh;

class cAnimationController : public iBehaviour
{
public:
	cAnimationController(iObject* parent) :
		iBehaviour(parent)
	{
	}

	virtual ~cAnimationController();

	bool deserialize(rapidxml::xml_node<>* root_node) override;

	// Inherited via iBehaviour
	virtual void start() override;

	virtual void update(float dt) override;

	void SetAnimation(std::string name);
	void SetAnimation(std::string name, float time);
	std::string GetAnimation();

	inline float GetCurrentTime() { return time; }
	float GetCurrentAnimationTime();
	float GetAnimationDuration();
	float GetAnimationDuration(std::string name);

	std::vector<std::string> GetAnimationNames();

	const std::vector< mat4 >& GetTransformations();

private:
	cSimpleAssimpSkinnedMesh* animation = 0;
	std::string currentAnimation = "";
	float time = 0.f;
	unsigned int numAnimations = 0;

	std::vector< mat4 > vecFinalTransformation;
	std::vector< mat4 > vecOffsets;
	std::vector< mat4 > vecObjectBoneTransformation;
};