#pragma once

class cRigidBody;
class cMeshRenderer;
class cMesh;

class iComponent
{
public:
	
	iComponent()
	{
		rigidBody		= 0;
		mesh			= 0;
		meshRenderer	= 0;
	}
	virtual ~iComponent() {}
	
	cRigidBody*		rigidBody;
	cMesh*			mesh;
	cMeshRenderer*	meshRenderer;
};