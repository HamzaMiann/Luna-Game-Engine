#pragma once

class cRigidBody;
class cMeshRenderer;

class iComponent
{
public:
	
	cRigidBody* rigidBody;
	cMeshRenderer* meshRenderer;
};