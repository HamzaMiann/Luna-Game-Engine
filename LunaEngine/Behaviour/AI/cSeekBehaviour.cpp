#include "cSeekBehaviour.h"
#include <EntityManager/cEntityManager.h>
#include <Physics/Mathf.h>

#pragma warning(disable)


void AI::cSeekBehaviour::SeekArrive()
{
    /*calculates the desired velocity */
            /*Seek uses target position - current position*/
            /*Flee uses current position - target position*/
    glm::vec3 desiredVelocity = target->pos - transform.pos;

    /* get the distance from target */
    float dist = glm::length(desiredVelocity);

    desiredVelocity = glm::normalize(desiredVelocity);

    desiredVelocity *= maxVelocity;

    /*is the game object within the radius around the target */
    //if (dist < slowingRadius)
    //{
    //    /* game object is approaching the target and slows down*/
    //    desiredVelocity = desiredVelocity * maxVelocity * (dist / slowingRadius);
    //}
    //else
    //{
    //    /* target is far away from game object*/
    //    desiredVelocity *= maxVelocity;
    //}

    float dot = 0.f;
    if (player_component)
        dot = glm::dot(glm::normalize(target->pos - transform.pos), player_component->direction);

    /*calculate the steering force */
    glm::vec3 steer = desiredVelocity - rb->GetVelocity();

    if (dot < 0.f)
        steer *= -0.7f;

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
    iObject* player = (iObject*)cEntityManager::Instance().GetObjectByTag("player");
    target = &player->transform;
    player_component = player->GetComponent<cPlayerBehaviour>();
    maxVelocity = 2.f;
}

void AI::cSeekBehaviour::update(float dt)
{
    mDt = dt;
    if (rb)
    {
        SeekArrive();
        glm::vec3 velocity = rb->GetVelocity();
        velocity.y = 0.f;
        velocity = glm::normalize(velocity);
        transform.rotation = Mathf::RotationFromTo(glm::vec3(0., 0., 1.), velocity);
    }
}
