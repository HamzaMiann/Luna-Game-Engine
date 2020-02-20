#include "cPursueBehaviour.h"
#include <EntityManager/cEntityManager.h>
#include <Physics/Mathf.h>
#include <Behaviour/Managers/cAIGameManager.h>

#pragma warning(disable)


typedef glm::vec3 vec3;

void AI::cPursueBehaviour::Pursue()
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

void AI::cPursueBehaviour::Evade(glm::vec3 position, glm::vec3 velocity)
{
    //calculate the number of frames we are looking ahead
    vec3 distance = position - transform.pos;
    int T = (int)glm::length(distance) / ((int)maxVelocity * 3);

    //the future target point the vehicle will pursue towards
    vec3 futurePosition = position + velocity * (float)T;

    /*calculates the desired velocity */
    vec3 desiredVelocity = futurePosition - transform.pos;

    desiredVelocity = glm::normalize(desiredVelocity);

    desiredVelocity *= maxVelocity * 3.f;

    /*calculate the steering force */
    vec3 steer = desiredVelocity - rb->GetVelocity();

    steer *= -1.f;

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
    iObject* player = (iObject*)cEntityManager::Instance().GetObjectByTag("player");
    target = &player->transform;
    target_rb = player->GetComponent<nPhysics::iPhysicsComponent>();
    manager = ((iObject*)cEntityManager::Instance().GetObjectByTag("manager"))->GetComponent<cAIGameManager>();
    maxVelocity = 3.f;
}

void AI::cPursueBehaviour::update(float dt)
{
    mDt = dt;
    if (rb && target_rb)
    {
        Pursue();

        float closest_distance = FLT_MAX;
        nPhysics::iPhysicsComponent* closest = 0;
        for (auto& bullet : manager->player_bullets)
        {
            if (closest == 0) closest = bullet;
            else
            {
                float d = glm::distance2(bullet->GetPosition(), transform.pos);
                if (d < closest_distance)
                {
                    closest = bullet;
                    closest_distance = d;
                }
            }
        }

        if (closest)
        {
            Evade(closest->GetPosition(), closest->GetVelocity());
        }

        glm::vec3 velocity = rb->GetVelocity();
        velocity.y = 0.f;
        velocity = glm::normalize(velocity);
        transform.rotation = Mathf::RotationFromTo(glm::vec3(0., 0., 1.), velocity);
    }
}
