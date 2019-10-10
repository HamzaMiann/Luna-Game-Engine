#include "cSpaceShip.h"

void cSpaceShip::Update()
{
	if (!target)
	{
		sNVPair message;
		message.name = "get closest planet";
		message.v3Value = this->positionXYZ;
		message = pWorld->ReceiveMessage(message);
		if (message.v3Ptr)
		{
			target = message.v3Ptr;
		}
	}
	else
	{
		cDebugRenderer* renderer = cDebugRenderer::GetInstance();
		renderer->addLine(this->positionXYZ,
						  this->positionXYZ + (*target - this->positionXYZ),
						  glm::vec3(0.f, 1.f, 1.f),
						  0.05f);
		renderer->addLine(this->previousXYZ,
						  this->positionXYZ,
						  glm::vec3(1.f, 0.f, 0.f),
						  0.05f);

		if (glm::distance2(*target, this->positionXYZ) < 0.5f)
		{
			sNVPair message;
			message.name = "harvest energy from planet";
			message.v3Ptr = this->target;
			message = pWorld->ReceiveMessage(message);
			if (message.name == "add energy")
			{

			}
			else
			{
				this->target = nullptr;
			}
		}
		else
		{
			glm::vec3 direction = glm::normalize(*target - this->positionXYZ);
			this->previousXYZ = this->positionXYZ;
			this->positionXYZ += direction * 0.05f;
		}
	}
	
}
