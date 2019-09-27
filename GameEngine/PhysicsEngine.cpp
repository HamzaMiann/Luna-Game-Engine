

#include "Scene.h"
#include "PhysicsEngine.h"
#include <math.h>
using namespace std;


// Alias to a type "existing type" "new type name"
typedef glm::vec3 Point;
typedef glm::vec3 Vector;

struct Sphere
{
	glm::vec3 c;
	float r;
};

// Closest point in 3D space. XYZ 
Point ClosestPtPointTriangle(Point p, Point a, Point b, Point c);
int TestSphereTriangle(Sphere s, Point a, Point b, Point c, Point& p);

void FindClosestPointToMesh(Scene& scene, float& closestDistanceSoFar, glm::vec3& closestPoint, glm::vec3& normalVector, cGameObject* const meshObject, cGameObject* const pObj)
{
	cMesh* mesh = scene.pVAOManager->FindMeshByModelName(meshObject->meshName);
	if (!mesh) return;

	for (unsigned int triIndex = 0;
		 triIndex != mesh->vecTriangles.size();
		 triIndex++)
	{
		sPlyTriangle& curTriangle = mesh->vecTriangles[triIndex];

		// Get the vertices of the triangle
		sPlyVertexXYZ triVert1 = mesh->vecVertices[curTriangle.vert_index_1];
		sPlyVertexXYZ triVert2 = mesh->vecVertices[curTriangle.vert_index_2];
		sPlyVertexXYZ triVert3 = mesh->vecVertices[curTriangle.vert_index_3];

		Point triVertPoint1;
		triVertPoint1.x = (triVert1.x * meshObject->scale + meshObject->positionXYZ.x);
		triVertPoint1.y = (triVert1.y * meshObject->scale + meshObject->positionXYZ.y);
		triVertPoint1.z = (triVert1.z * meshObject->scale + meshObject->positionXYZ.z);

		Point triVertPoint2;
		triVertPoint2.x = (triVert2.x * meshObject->scale + meshObject->positionXYZ.x);
		triVertPoint2.y = (triVert2.y * meshObject->scale + meshObject->positionXYZ.y);
		triVertPoint2.z = (triVert2.z * meshObject->scale + meshObject->positionXYZ.z);

		Point triVertPoint3;
		triVertPoint3.x = (triVert3.x * meshObject->scale + meshObject->positionXYZ.x);
		triVertPoint3.y = (triVert3.y * meshObject->scale + meshObject->positionXYZ.y);
		triVertPoint3.z = (triVert3.z * meshObject->scale + meshObject->positionXYZ.z);

		glm::vec3 curClosetPoint = ClosestPtPointTriangle(
			pObj->positionXYZ,
			triVertPoint1, triVertPoint2, triVertPoint3
		);

		// Is this the closest so far?
		float distanceNow = glm::distance(curClosetPoint, pObj->positionXYZ);

		// is this closer than the closest distance
		if (distanceNow <= closestDistanceSoFar)
		{
			closestDistanceSoFar = distanceNow;
			closestPoint = curClosetPoint;
			normalVector.x = (triVert1.nx + triVert2.nx + triVert3.nx) / 3.f;
			normalVector.y = (triVert1.ny + triVert2.ny + triVert3.ny) / 3.f;
			normalVector.z = (triVert1.nz + triVert2.nz + triVert3.nz) / 3.f;
		}
	}
}


void PhysicsEngine::IntegrationStep(Scene* scene, float deltaTime)
{

	for (size_t i = 0; i < scene->vecGameObjects.size(); ++i)
	{
		// Forward Explicit Euler Integration
		cGameObject* pObj = (scene->vecGameObjects[i]);

		// if infinite mass, don't run physics
		if (pObj->inverseMass == 0.f) continue;

		// add acceleration
		pObj->velocity += (pObj->acceleration * deltaTime);

		// add external forces
		pObj->velocity += (Gravity * deltaTime);
		pObj->velocity *= 1.f - (drag * deltaTime);

		float speed = pObj->velocity.y;
		//printf("%f\n", speed);

		if (pObj->isCollided)
		{
			// glm::distance(pObj->velocity, glm::vec3(0.f));
			if (speed <= 0.5f)
			{
				pObj->velocity.y *= friction;
			}
			if (speed <= 0.4f)
			{
				pObj->velocity.y *= friction;
			}
			if (speed <= 0.25f)
			{
				pObj->velocity.y = 0.f;//glm::vec3(0.f);
				//pObj->velocity.x *= friction;
				//pObj->velocity.z *= friction;
			}
		}

		/*if (glm::distance(glm::vec3(0.f), (pObj->positionXYZ + (pObj->velocity * deltaTime))) <= 0.5f)
		{
			float lastY = glm::distance(glm::vec3(0.f), pObj->positionXYZ);
			float newY = glm::distance(glm::vec3(0.f), (pObj->positionXYZ + (pObj->velocity * deltaTime)));
			if (abs(lastY - newY) <= 0.5f * deltaTime) pObj->velocity *= friction;
			if (abs(lastY - newY) <= 0.4f * deltaTime) pObj->velocity *= friction;
			if (abs(lastY - newY) <= 0.2f * deltaTime)
			{
				pObj->velocity = glm::vec3(0.f);
			}
			pObj->velocity.x = pObj->velocity.x * drag;
			pObj->velocity.y = pObj->velocity.y * drag;
			pObj->velocity.z = pObj->velocity.z * drag;
		}*/
		pObj->positionXYZ += (pObj->velocity * deltaTime);

		pObj->isCollided = false;

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

void PhysicsEngine::CheckCollisions(Scene* scene)
{
	for (size_t i = 0; i < scene->vecGameObjects.size(); ++i)
	{
		// object to check collisions on
		cGameObject* pObj = (scene->vecGameObjects[i]);

		// if infinite mass, don't run physics
		if (pObj->inverseMass == 0.f) continue;

		for (int k = 0; k < scene->vecGameObjects.size(); ++k)
		{
			if (k != i && scene->vecGameObjects[k]->Collider != NONE)
			{
				cGameObject* colliderObject = scene->vecGameObjects[k];
				float closestDistanceSoFar = FLT_MAX;
				glm::vec3 closestPoint = glm::vec3(0.0f, 0.0f, 0.0f);
				glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
				switch (colliderObject->Collider)
				{
				case MESH:
					
					FindClosestPointToMesh(*scene, closestDistanceSoFar, closestPoint, normal, colliderObject, pObj);

					if (pObj->Collider == SPHERE)
						closestDistanceSoFar -= pObj->scale;
					
					if (abs(closestDistanceSoFar) <= 0.1f)
					{
						//printf("%f\n", glm::dot(pObj->velocity, normal));
						if (glm::dot(pObj->velocity, normal) > 0)
							break;
						//if (closestDistanceSoFar < 0) pObj->positionXYZ = closestPoint;
						if (!pObj->isCollided)
						{
							pObj->velocity = glm::reflect(pObj->velocity, normal) /** friction*/;
							pObj->isCollided |= true;
							AudioEngine::Sound* sound = scene->pAudioEngine->GetSound("ball");
							if (sound)
							{
								float volume = glm::distance(pObj->velocity, glm::vec3(0.f));
								volume = volume > 1.f ? 1.f : volume;
								sound->set_volume(volume);
								scene->pAudioEngine->PlaySound(sound);
							}
						}
					}
					else
					{
						pObj->isCollided |= false;
					}

					break;
				case SPHERE:
					if (pObj->Collider == SPHERE)
					{
						float distance = glm::distance(pObj->positionXYZ, colliderObject->positionXYZ);
						distance -= (pObj->scale + colliderObject->scale) / 2.f;
						if (distance <= 0.1f)
						{
							//pObj->acceleration = (pObj->acceleration) * -1.f;
							//colliderObject->acceleration = (colliderObject->acceleration) * -1.f;
							if (!pObj->isCollided)
							{
								pObj->velocity = (pObj->velocity) * -1.f;
								colliderObject->velocity = (colliderObject->velocity) * -1.f;
								pObj->isCollided = true;
							}
						}
						else
						{
							pObj->isCollided = false;
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
	}
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