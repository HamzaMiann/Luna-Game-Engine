#include "cPursueBehaviour.h"
#include <cGameObject.h>
#include <EntityManager/cEntityManager.h>

typedef glm::vec3 vec3;

void AI::cPursueBehaviour::PursueEvade()
{
    //calculate the number of frames we are looking ahead
    vec3 distance = target->pos - transform.pos;
    int T = (int)glm::length(distance) / (int)maxVelocity;

    //the future target point the vehicle will pursue towards
    vec3 futurePosition = target->pos + target_rb->GetVelocity() * (float)T;

    /*calculates the desired velocity */
    vec3 desiredVelocity = futurePosition - transform.pos;

    desiredVelocity = glm::normalize(desiredVelocity);

    desiredVelocity *= maxVelocity;

    /*calculate the steering force */
    vec3 steer = desiredVelocity - rb->GetVelocity();

    /* add steering force to current velocity*/
    rb->SetVelocity(rb->GetVelocity() + steer * mDt);

    if (glm::length(rb->GetVelocity()) > maxVelocity)
    {
        rb->SetVelocity(glm::normalize(rb->GetVelocity()) * maxVelocity);
    }
}

bool AI::cPursueBehaviour::serialize(rapidxml::xml_node<>* root_node)
{
    return false;
}

bool AI::cPursueBehaviour::deserialize(rapidxml::xml_node<>* root_node)
{
    return false;
}

void AI::cPursueBehaviour::start()
{
    rb = parent.GetComponent<nPhysics::iPhysicsComponent>();
    cGameObject* player = cEntityManager::Instance()->GetObjectByTag("player");
    target = &player->transform;
    target_rb = player->GetComponent<nPhysics::iPhysicsComponent>();
    maxVelocity = 10.f;
}

void AI::cPursueBehaviour::update(float dt)
{
    mDt = dt;
    if (rb && target_rb)
    {
        PursueEvade();
    }
}
