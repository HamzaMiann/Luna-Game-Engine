#pragma once

#include "../bullet/btBulletDynamicsCommon.h"

class cBulletWrapperComponent
{
public:
	virtual ~cBulletWrapperComponent() {}
	btRigidBody* mBody;

protected:
	cBulletWrapperComponent() : mBody(0) {}

};