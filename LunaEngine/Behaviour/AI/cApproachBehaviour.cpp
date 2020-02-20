#include "cApproachBehaviour.h"
#include <EntityManager/cEntityManager.h>
#include <Physics/Mathf.h>
#include <Behaviour/Managers/cAIGameManager.h>

#pragma warning(disable)

void AI::cApproachBehaviour::Approach()
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

bool AI::cApproachBehaviour::serialize(rapidxml::xml_node<>* root_node)
{
    return false;
}

bool AI::cApproachBehaviour::deserialize(rapidxml::xml_node<>* root_node)
{
    return false;
}

void AI::cApproachBehaviour::start()
{
    rb = parent.GetComponent<nPhysics::iPhysicsComponent>();
    iObject* player = cEntityManager::Instance().GetObjectByTag("player");
    target = &player->transform;
    player_component = player->GetComponent<cPlayerBehaviour>();
    slowingRadius = 8.f;
    maxVelocity = 2.f;
    reload_time = -1.f;
}

void AI::cApproachBehaviour::update(float dt)
{
    mDt = dt;
    if (rb)
    {
        Approach();
        glm::vec3 velocity = rb->GetVelocity();
        velocity.y = 0.f;
        velocity = glm::normalize(velocity);
        transform.rotation = Mathf::RotationFromTo(glm::vec3(0., 0., 1.), velocity);
        if (reload_time < 0.f && distance(transform.pos, target->pos) < slowingRadius)
        {
            glm::vec3 direction = glm::normalize(target->pos - transform.pos);
            reload_time = 3.f;
            iObject* obj = cEntityManager::Instance().GetObjectByTag("manager");
            cAIGameManager* manager = obj->GetComponent<cAIGameManager>();
            if (manager)
            {
                manager->Enemy_Shoot(transform.pos + direction * 1.5f, direction * 3.f);
            }
        }
    }
    reload_time -= dt;
}
