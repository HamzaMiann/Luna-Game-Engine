

#include "Scene.h"
#include "PhysicsEngine.h"
#include <math.h>
using namespace std;


// Alias to a type "existing type" "new type name"
typedef glm::vec3 Point;
typedef glm::vec3 Vector;

// Closest point in 3D space. XYZ 
Point ClosestPtPointTriangle(Point p, Point a, Point b, Point c);

struct Sphere
{
	glm::vec3 c;
	float r;
};
int TestSphereTriangle(Sphere s, Point a, Point b, Point c, Point& p);


//void FindClosestPointToMesh(Scene& scene, float& closestDistanceSoFar, glm::vec3& closetPoint)
//{
//	for (unsigned int triIndex = 0;
//		 triIndex != terrainMesh.vecTriangles.size();
//		 triIndex++)
//	{
//		sPlyTriangle& curTriangle = terrainMesh.vecTriangles[triIndex];
//
//		// Get the vertices of the triangle
//		sPlyVertexXYZ_N triVert1 = terrainMesh.vecVertices[curTriangle.vert_index_1];
//		sPlyVertexXYZ_N triVert2 = terrainMesh.vecVertices[curTriangle.vert_index_2];
//		sPlyVertexXYZ_N triVert3 = terrainMesh.vecVertices[curTriangle.vert_index_3];
//
//		Point triVertPoint1;
//		triVertPoint1.x = triVert1.x;
//		triVertPoint1.y = triVert1.y;
//		triVertPoint1.z = triVert1.z;
//
//		Point triVertPoint2;
//		triVertPoint2.x = triVert2.x;
//		triVertPoint2.y = triVert2.y;
//		triVertPoint2.z = triVert2.z;
//
//		Point triVertPoint3;
//		triVertPoint3.x = triVert3.x;
//		triVertPoint3.y = triVert3.y;
//		triVertPoint3.z = triVert3.z;
//
//		glm::vec3 curClosetPoint = ClosestPtPointTriangle(
//			pShpere->positionXYZ,
//			triVertPoint1, triVertPoint2, triVertPoint3);
//
//		// Is this the closest so far?
//		float distanceNow = glm::distance(curClosetPoint, pShpere->positionXYZ);
//
//		// is this closer than the closest distance
//		if (distanceNow <= closestDistanceSoFar)
//		{
//			closestDistanceSoFar = distanceNow;
//			closetPoint = curClosetPoint;
//		}
//
//		//glm::mat4 matModel = glm::mat4(1.0f);
//		//pDebugSphere->positionXYZ = closetPoint;
//		//DrawObject(matModel, pDebugSphere, 
//		//			   shaderProgID, pTheVAOManager);
//
//
//	}//for (unsigned int triIndex = 0;
//}


void PhysicsUpdate(Scene* scene, float deltaTime)
{
	glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f);
	float friction = .7f;
	float drag = .8f;

	for (size_t i = 0; i < scene->vecGameObjects.size(); ++i)
	{
		// Forward Explicit Euler Integration
		cGameObject* pObj = (scene->vecGameObjects[i]);

		// if infinite mass, don't run physics
		if (pObj->inverseMass == 0.f) continue;

		// add acceleration and external forces
		pObj->velocity += (pObj->acceleration * deltaTime);
		pObj->velocity += (gravity * deltaTime);

		
		for (int k = 0; k < scene->vecGameObjects.size(); ++k)
		{
			if (k != i && scene->vecGameObjects[k]->Collider != NONE)
			{
				cGameObject* colliderObject = scene->vecGameObjects[k];
				switch (colliderObject->Collider)
				{
				case MESH:
					//float closestDistanceSoFar = FLT_MAX;
					//glm::vec3 closetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
					break;
				case SPHERE:
					if (pObj->Collider == SPHERE)
					{
						float distance = glm::distance(pObj->positionXYZ, colliderObject->positionXYZ);
						distance -= (pObj->scale + colliderObject->scale);
						if (distance <= 0.1f)
						{
							//pObj->acceleration = (pObj->acceleration) * -1.f;
							//colliderObject->acceleration = (colliderObject->acceleration) * -1.f;

							pObj->velocity = (pObj->velocity) * -1.f;
							colliderObject->velocity = (colliderObject->velocity) * -1.f;
						}
					}
					break;
				case CUBE:
					break;
				case NONE:
				default:
					break;
				}
			}
		}

		if ((pObj->positionXYZ.y + (pObj->velocity * deltaTime)).y <= 0.5f)
		{
			bool shouldPlaySound = true;
			float lastY = pObj->positionXYZ.y;
			float newY = (pObj->positionXYZ.y + (pObj->velocity * deltaTime)).y;
			if (abs(lastY - newY) <= 0.5f * deltaTime) pObj->velocity.y *= friction;
			if (abs(lastY - newY) <= 0.4f * deltaTime) pObj->velocity.y *= friction;
			if (abs(lastY - newY) <= 0.2f * deltaTime)
			{
				pObj->velocity.y = 0.f;
				shouldPlaySound = false;
			}
			pObj->velocity.y = pObj->velocity.y * -1.f * friction;
			pObj->velocity.x = pObj->velocity.x * drag;
			pObj->velocity.z = pObj->velocity.z * drag;
			if (shouldPlaySound)
			{
				scene->pAudioEngine->PlaySound("ball");
			}
		}
		pObj->positionXYZ += (pObj->velocity * deltaTime);

		

		// Test to see if it's hit the cube

		// Determine the object new position

		// Position ( x = 0 )

		// Velocity ( Vx = 1.0f / second )

		// DeltaX = Vx * deltaTime
		//		= 1.0f / sec * 1.0 second

		// Position += DeltaX
		// Position += Vx * deltaTime

		// Velocity += DeltaV * detlaTime
		// Velocity += Ax * deltaTime

	}

	return;
}


// Closest point in 3D space. XYZ 
Point ClosestPtPointTriangle(Point p, Point a, Point b, Point c)
{

	Vector ab = b - a;
	Vector ac = c - a;
	Vector bc = c - b;

	// Compute parametric position s for projection P' of P on AB,
	// P' = A + s*AB, s = snom/(snom+sdenom)
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);

	// Compute parametric position t for projection P' of P on AC,
	// P' = A + t*AC, s = tnom/(tnom+tdenom)
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P' of P on BC,
	// P' = B + u*BC, u = unom/(unom+udenom)
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out


	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
	Vector n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));
	// If P outside AB and within feature region of AB,
	// return projection of P onto AB
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC,
	// return projection of P onto BC
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
	float vb = glm::dot(n, glm::cross(c - p, a - p));
	// If P outside CA and within feature region of CA,
	// return projection of P onto CA
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc)
	return u * a + v * b + w * c;
}
// From Ericson's book:
// Returns true if sphere s intersects triangle ABC, false otherwise.
// The point p on abc closest to the sphere center is also returned
//struct Sphere
//{
//	glm::vec3 c;
//	float r;
//};
int TestSphereTriangle(Sphere s, Point a, Point b, Point c, Point& p)
{
	// Find point P on triangle ABC closest to sphere center
	p = ClosestPtPointTriangle(s.c, a, b, c);

	// Sphere and triangle intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	Vector v = p - s.c;
	return glm::dot(v, v) <= s.r * s.r;
}