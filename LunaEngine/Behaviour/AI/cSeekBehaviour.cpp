#include "cSeekBehaviour.h"
#include <cGameObject.h>
#include <EntityManager/cEntityManager.h>

void AI::cSeekBehaviour::SeekArrive()
{
    /*calculates the desired velocity */
            /*Seek uses target position - current position*/
            /*Flee uses current position - target position*/
    glm::vec3 desiredVelocity = target->pos - transform.pos;

    /* get the distance from target */
    float dist = glm::length(desiredVelocity);

    desiredVelocity = glm::normalize(desiredVelocity);

    /*is the game object within the radius around the target */
    if (dist < slowingRadius)
    {
        /* game object is approaching the target and slows down*/
        desiredVelocity = desiredVelocity * maxVelocity * (dist / slowingRadius);
    }
    else
    {
        /* target is far away from game object*/
        desiredVelocity *= maxVelocity;
    }

    /*calculate the steering force */
    glm::vec3 steer = desiredVelocity - rb->GetVelocity();

    /* add steering force to current velocity*/
    rb->SetVelocity(rb->GetVelocity() + steer * mDt);

    if (glm::length(rb->GetVelocity()) > maxVelocity)
    {
        rb->SetVelocity(glm::normalize(rb->GetVelocity()) * maxVelocity);
    }
}

bool AI::cSeekBehaviour::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool AI::cSeekBehaviour::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

void AI::cSeekBehaviour::start()
{
    rb = parent.GetComponent<nPhysics::iPhysicsComponent>();
    cGameObject* player = cEntityManager::Instance()->GetObjectByTag("player");
    target = &player->transform;
    slowingRadius = 5.f;
    maxVelocity = 10.f;
}

void AI::cSeekBehaviour::update(float dt)
{
    mDt = dt;
    if (rb)
    {
        SeekArrive();
    }
}
