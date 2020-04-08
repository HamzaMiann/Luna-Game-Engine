#pragma once

#include "../bullet/btBulletDynamicsCommon.h"
#include <interfaces/physics/iPhysicsComponent.h>



class cBulletWrapperComponent
{
public:
	virtual ~cBulletWrapperComponent() {}
	btRigidBody* mBody;

protected:
	cBulletWrapperComponent() : mBody(0) {}

};