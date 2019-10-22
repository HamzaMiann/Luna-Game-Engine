#include "cSpaceShipImplementation.h"
#include "cDebugRenderer.h"

sNVPair cSpaceShipImplementation::ReceiveMessage(sNVPair message)
{
	sNVPair returnMessage("ok", 0.f);
	if (message.name == "take damage")
	{
		health -= message.fValue;
		printf("Taking damage! Health left: %f\n", health);
		if (health <= 0.f)
		{
			returnMessage.name = "dead";
			printf("--DEAD--\n");
		}
	}
	return returnMessage;
}

void cSpaceShipImplementation::Update(iMessageInterface * mediator, cGameObject * self)
{
	cDebugRenderer* renderer = cDebugRenderer::GetInstance();

	sNVPair enemy = mediator->ReceiveMessage(sNVPair("get closest enemy", &self->positionXYZ));
	if (enemy.v3Ptr && glm::distance(self->positionXYZ, *enemy.v3Ptr) < 3.f)
	{
		printf("Shooting a frickin' lazer beam. Damage: %f\n", damage);
		renderer->addLine(self->positionXYZ,
						  *enemy.v3Ptr,
						  glm::vec3(1.f, 0.f, 0.f),
						  0.1f);
		enemy.name = "attack enemy";
		enemy.fValue = damage;
		mediator->ReceiveMessage(enemy);
	}
	else if (!target)
	{
		sNVPair message;
		message.name = "get closest planet";
		message.v3Value = self->positionXYZ;
		message = mediator->ReceiveMessage(message);
		if (message.v3Ptr)
		{
			target = message.v3Ptr;
		}
	}
	else
	{
		renderer->addLine(self->positionXYZ,
						  self->positionXYZ + (*target - self->positionXYZ),
						  glm::vec3(0.f, 1.f, 1.f),
						  0.05f);
		renderer->addLine(self->previousXYZ,
						  self->positionXYZ,
						  glm::vec3(0.f, 1.f, 0.f),
						  0.1f);

		if (glm::distance(*target, self->positionXYZ) < 0.5f)
		{
			sNVPair message;
			message.name = "harvest energy from planet";
			message.v3Ptr = target;
			message = mediator->ReceiveMessage(message);
			if (message.name == "add energy")
			{
				damage *= 1.005f;
				health += message.fValue;
			}
			else
			{
				target = nullptr;
			}
		}
		else
		{
			glm::vec3 direction = glm::normalize(*target - self->positionXYZ);
			self->previousXYZ = self->positionXYZ;
			self->positionXYZ += direction * 0.05f;
		}
	}
}


sNVPair cDoNothingImpl::ReceiveMessage(sNVPair message)
{
	sNVPair returnMessage("ok", 0.f);
	if (message.name == "take damage")
	{
		health -= message.fValue;
		printf("Taking damage! Health left: %f\n", health);
		if (health <= 0.f)
		{
			returnMessage.name = "dead";
			printf("--DEAD--\n");
		}
	}
	return returnMessage;
}

void cDoNothingImpl::Update(iMessageInterface * mediator, cGameObject * self)
{
	mediator->ReceiveMessage(sNVPair("attack all", 0.f));
}
