

#include "Scene.h"
#include "PhysicsEngine.h"
#include "cMesh.h"
#include "cVAOManager.h"
#include "cModelLoader.h"
#include "cGameObject.h"
#include <math.h>
#include <algorithm>
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
void FindClosestPointToMesh(Scene& scene, float& closestDistanceSoFar, glm::vec3& closestPoint, glm::vec3& normalVector, cGameObject* const meshObject, cGameObject* const pObj);
void sphereCollisionResponse(cGameObject* a, cGameObject* b, PhysicsEngine* phys);

void PhysicsEngine::IntegrationStep(Scene* scene, float deltaTime)
{

	for (size_t i = 0; i < scene->vecGameObjects.size(); ++i)
	{
		// Forward Explicit Euler Integration
		cGameObject* pObj = (scene->vecGameObjects[i]);

		pObj->previousPos = pObj->pos;

		// if infinite mass, don't run physics
		if (pObj->inverseMass == 0.f) continue;

		// add acceleration
		pObj->velocity += (pObj->acceleration * deltaTime);

		// add external forces
		pObj->velocity += (Gravity * deltaTime * pObj->gravityScale);
		pObj->velocity *= 1.f - (drag * deltaTime);

		float speed = abs(pObj->velocity.y);
		//printf("%f\n", speed);

		if (pObj->isCollided)
		{
			// glm::distance(pObj->velocity, glm::vec3(0.f));
			if (speed <= 0.5f)
			{
				pObj->velocity.y *= friction;
			}
			speed = abs(pObj->velocity.y);
			if (speed <= 0.4f)
			{
				pObj->velocity.y *= friction;
			}
			speed = abs(pObj->velocity.y);
			if (speed <= 0.2f)
			{
				pObj->velocity.y = 0.f;//glm::vec3(0.f);
				//pObj->velocity.x *= friction;
				//pObj->velocity.z *= friction;
			}
		}

		/*if (glm::distance(glm::vec3(0.f), (pObj->pos + (pObj->velocity * deltaTime))) <= 0.5f)
		{
			float lastY = glm::distance(glm::vec3(0.f), pObj->pos);
			float newY = glm::distance(glm::vec3(0.f), (pObj->pos + (pObj->velocity * deltaTime)));
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
		pObj->pos += (pObj->velocity * deltaTime);
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
struct cPair
{
	cPair(unsigned int _first, unsigned int _second) :
		first(_first), second(_second)
	{
	}
	unsigned int first = 0;
	unsigned int second = 0;
};
void PhysicsEngine::CheckCollisions(Scene* scene)
{
	std::vector<cPair> collisionHistory;

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
				bool shouldContinue = false;
				for (unsigned int j = 0; j < collisionHistory.size(); ++j)
				{
					if (collisionHistory[j].first == k && collisionHistory[j].second == i)
					{
						shouldContinue = true;
						break;
					}
				}
				if (shouldContinue) continue;

				collisionHistory.push_back(cPair(i, k));

				cGameObject* colliderObject = scene->vecGameObjects[k];
				float closestDistanceSoFar = FLT_MAX;
				glm::vec3 closestPoint = glm::vec3(0.0f, 0.0f, 0.0f);
				glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
				switch (colliderObject->Collider)
				{
				case MESH:
					if (pObj->Collider == SPHERE)
					{
						FindClosestPointToMesh(*scene, closestDistanceSoFar, closestPoint, normal, colliderObject, pObj);

						closestDistanceSoFar -= pObj->scale;

						if (abs(closestDistanceSoFar) <= 0.08f)
						{
							if (glm::dot(glm::normalize(Gravity), normal) == 0)
							{
								if (colliderObject->inverseMass != 0.0f)
								{
									pObj->velocity += colliderObject->velocity;
								}
							}


							if (glm::dot(pObj->velocity, normal) > 0)
								break;

							if (closestDistanceSoFar < 0.f)
								pObj->pos.y = pObj->previousPos.y;

							pObj->velocity = glm::reflect(pObj->velocity, normal) /** friction*/;
							pObj->isCollided |= true;


							if (renderer)
							{
								renderer->addLine(closestPoint,
												  closestPoint + normal,
												  glm::vec3(0.f, 1.f, 1.f),
												  1.f);

								renderer->addLine(closestPoint,
												  closestPoint + glm::normalize(pObj->velocity),
												  glm::vec3(0.f, 1.f, 0.f),
												  1.f);
							}
						}

					}
					break;
				case SPHERE:
				{
					if (pObj->Collider == SPHERE)
					{
						float distance = glm::distance(pObj->pos, colliderObject->pos);
						distance -= (pObj->scale + colliderObject->scale);// / 2.f;
						if (distance <= 0.1f)
						{
							sphereCollisionResponse(pObj, colliderObject, this);
						}
					}
				}
					break;
				case AABB:
					if (pObj->Collider == SPHERE)
					{
						octree::octree_node* node = tree->find_node(pObj->pos);
						if (node)
						{
							FindClosestPointToTriangles(node->triangles, closestDistanceSoFar, closestPoint, normal, pObj->pos);

							closestDistanceSoFar -= pObj->scale;

							if (abs(closestDistanceSoFar) <= 0.08f)
							{
								if (glm::dot(glm::normalize(Gravity), normal) == 0)
								{
									if (colliderObject->inverseMass != 0.0f)
									{
										pObj->velocity += colliderObject->velocity;
									}
								}


								if (glm::dot(pObj->velocity, normal) > 0)
									break;

								if (closestDistanceSoFar < 0.f)
									pObj->pos.y = pObj->previousPos.y;

								pObj->velocity = glm::reflect(pObj->velocity, normal) /** friction*/;
								pObj->isCollided |= true;


								if (renderer)
								{
									renderer->addLine(closestPoint,
													  closestPoint + normal,
													  glm::vec3(0.f, 1.f, 1.f),
													  1.f);

									renderer->addLine(closestPoint,
													  closestPoint + glm::normalize(pObj->velocity),
													  glm::vec3(0.f, 1.f, 0.f),
													  1.f);
								}
							}
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


void PhysicsEngine::GenerateAABB(Scene* scene)
{
	glm::vec3 min = scene->pModelLoader->min;
	glm::vec3 max = scene->pModelLoader->max;

	tree = new octree;
	tree->generate_tree(min, glm::distance(min, max));

	for (size_t i = 0; i < scene->vecGameObjects.size(); ++i)
	{
		if (scene->vecGameObjects[i]->Collider == AABB)
		{
			cMesh* mesh = scene->pVAOManager->FindMeshByModelName(scene->vecGameObjects[i]->meshName);
			if (mesh)
			{
				tree->attach_triangles(mesh->vecMeshTriangles);
			}
		}
	}
}


void FindClosestPointToMesh(Scene& scene, float& closestDistanceSoFar, glm::vec3& closestPoint, glm::vec3& normalVector, cGameObject* const meshObject, cGameObject* const pObj)
{
	cMesh* mesh = scene.pVAOManager->FindMeshByModelName(meshObject->meshName);
	if (!mesh) return;

	glm::mat4 transform(1.f);
	transform *= glm::rotate(glm::mat4(1.0f),
							 meshObject->rotation.z,
							 glm::vec3(0.0f, 0.0f, 1.0f));
	transform *= glm::rotate(glm::mat4(1.0f),
							 meshObject->rotation.y,
							 glm::vec3(0.0f, 1.0f, 0.0f));
	transform *= glm::rotate(glm::mat4(1.0f),
							 meshObject->rotation.x,
							 glm::vec3(1.0f, 0.0f, 0.0f));

	for (unsigned int i = 0; i < mesh->vecMeshTriangles.size(); ++i)
	{
		sMeshTriangle curTriangle = mesh->vecMeshTriangles[i];

		glm::vec3 curClosetPoint = ClosestPtPointTriangle(
			pObj->pos,
			glm::vec3(transform * glm::vec4(curTriangle.first, 1.f)) * meshObject->scale + meshObject->pos,
			glm::vec3(transform * glm::vec4(curTriangle.second, 1.f)) * meshObject->scale + meshObject->pos,
			glm::vec3(transform * glm::vec4(curTriangle.third, 1.f)) * meshObject->scale + meshObject->pos
		);

		// Is this the closest so far?
		float distanceNow = glm::distance(curClosetPoint, pObj->pos);

		// is this closer than the closest distance
		if (distanceNow <= closestDistanceSoFar)
		{
			closestDistanceSoFar = distanceNow;
			closestPoint = curClosetPoint;
			normalVector = transform * glm::vec4(curTriangle.normal, 1.f);
		}
	}
}

void FindClosestPointToTriangles(std::vector<const sMeshTriangle*> const& triangles, float& closestDistanceSoFar, glm::vec3& closestPoint, glm::vec3& normalVector, glm::vec3 point)
{

	for (unsigned int i = 0; i < triangles.size(); ++i)
	{
		const sMeshTriangle* curTriangle = triangles[i];

		glm::vec3 curClosetPoint = ClosestPtPointTriangle(
			point,
			curTriangle->first,
			curTriangle->second,
			curTriangle->third
		);

		// Is this the closest so far?
		float distanceNow = glm::distance(curClosetPoint, point);

		// is this closer than the closest distance
		if (distanceNow <= closestDistanceSoFar)
		{
			closestDistanceSoFar = distanceNow;
			closestPoint = curClosetPoint;
			normalVector = curTriangle->normal;
		}
	}
}


// code for collision response between spheres
void sphereCollisionResponse(cGameObject* a, cGameObject* b, PhysicsEngine* phys)
{
	glm::vec3 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y;

	float m1, m2, x1, x2;
	glm::vec3 v1temp, v1, v2, v1x, v2x, v1y, v2y, x(a->pos - b->pos);

	glm::normalize(x);
	v1 = a->velocity;
	x1 = dot(x, v1);
	v1x = x * x1;
	v1y = v1 - v1x;
	m1 = 1.0f; //mass of 1

	x = x * -1.0f;
	v2 = b->velocity;
	x2 = dot(x, v2);
	v2x = x * x2;
	v2y = v2 - v2x;
	m2 = 1.0f; //mass of 1


	a->velocity = glm::vec3(v1x * (m1 - m2) / (m1 + m2) + v2x * (2 * m2) / (m1 + m2) + v1y) / 4.0f;
	b->velocity = glm::vec3(v1x * (2 * m1) / (m1 + m2) + v2x * (m2 - m1) / (m1 + m2) + v2y) / 4.0f;

	if (glm::length(a->velocity) < 0.2f);
	a->velocity *= 5.f;
	if (glm::length(b->velocity) < 0.2f);
	b->velocity *= 5.f;

	/*float distance = glm::distance(a->previousPos, b->previousPos) - (a->scale + b->scale);
	if (distance <= 0.1f)
	{
		a->pos = a->pos + (b->previousPos - a->previousPos) * 1.5f;
		b->pos = b->pos + (b->previousPos - a->previousPos) * -1.5f;
		a->previousPos = a->pos;
		b->previousPos = b->pos;
	}*/

	//set the position of the spheres to their previous non contact positions to unstick them.
	a->pos = a->previousPos;
	b->pos = b->previousPos;

	if (phys->renderer)
	{
		phys->renderer->addLine(a->pos,
								a->pos + a->velocity,
								glm::vec3(.5f, .9f, .5f),
								.1f);

		phys->renderer->addLine(b->pos,
								b->pos + b->velocity,
								glm::vec3(.5f, .9f, .5f),
								.1f);
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

float ScalarTriple(Vector a, Vector b, Vector c)
{
	return glm::dot(glm::cross(a, b), c);
}

//Given line pq and ccw triangle abc, return whether line pierces triangle. If
//so, also return the barycentric coordinates (u,v,w) of the intersection point
int IntersectLineTriangle(Point p, Point q, Point a, Point b, Point c, Point &r)//, float &u, float &v, float &w)
{
	float u = 0.f, v = 0.f, w = 0.f;
	Vector pq = q - p; Vector pa = a - p;
	Vector pb = b - p; Vector pc = c - p;
	// Test if pq is inside the edges bc, ca and ab. Done by testing
	// that the signed tetrahedral volumes, computed using scalar triple
	// products, are all positive
	u = ScalarTriple(pq, pc, pb);
	if (u < 0.0f) return 0;
	v = ScalarTriple(pq, pa, pc);
	if (v < 0.0f) return 0;
	w = ScalarTriple(pq, pb, pa);
	if (w < 0.0f) return 0;
	// Compute the barycentric coordinates (u, v, w) determining the
	// intersection point r, r = u*a + v*b + w*c
	float denom = 1.0f / (u + v + w);
	u *= denom;
	v *= denom;
	w *= denom; // w = 1.0f - u - v;

	r = Point(u, v, w);

	return 1;
}