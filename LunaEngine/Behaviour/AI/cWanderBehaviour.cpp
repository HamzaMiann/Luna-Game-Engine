#include "cWanderBehaviour.h"
#include <cGameObject.h>
#include <EntityManager/cEntityManager.h>
#include <Physics/Mathf.h>

typedef glm::vec3 vec3;

void AI::cWanderBehaviour::Wander()
{
    vec3 velocity = rb->GetVelocity();
    vec3 circlePoint;
    if (glm::length(velocity) <= 0.01f)
    {
        circlePoint = glm::vec3(0.f, 0.f, 1.f) * distanceToCircle + transform.pos;
    }
    else
    {
        velocity.y = 0.f;
        circlePoint = glm::normalize(velocity) * distanceToCircle + transform.pos;
    }

    //calculate a random spot on the circle's circumference
    float angle = Mathf::randInRange(0.f, PI * 2);
    float x = Mathf::Sin(angle) * circleRadius;
    float z = Mathf::Cos(angle) * circleRadius;

    //the target point the wandering vehicle will seek towards
    vec3 targetPosition = vec3(circlePoint.x + x, 0.5f, circlePoint.z + z);

    /*calculates the desired velocity */
    vec3 desiredVelocity = targetPosition - transform.pos;

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

bool AI::cWanderBehaviour::serialize(rapidxml::xml_node<>* root_node)
{
    return false;
}

bool AI::cWanderBehaviour::deserialize(rapidxml::xml_node<>* root_node)
{
    return false;
}

void AI::cWanderBehaviour::start()
{
    rb = parent.GetComponent<nPhysics::iPhysicsComponent>();
    maxVelocity = 5.f;
    distanceToCircle = 2.f;
    circleRadius = 5.f;
}

void AI::cWanderBehaviour::update(float dt)
{
    mDt = dt;
    if (rb && wander_time < 6.f)
    {
        Wander();
        wander_time += dt;
        if (wander_time > 6.f)
        {
            idle_time = 0.f;
        }
    }
    else if (rb && idle_time < 3.f)
    {
        rb->SetVelocity(rb->GetVelocity() * 0.1f * dt);
        idle_time += dt;
        if (idle_time > 3.f)
        {
            wander_time = 0.f;
        }
    }
}
