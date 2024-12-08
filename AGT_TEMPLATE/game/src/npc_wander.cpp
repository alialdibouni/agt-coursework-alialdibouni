// = The code written has been assisted with Github Copilot AI = //

#include "pch.h"
#include "npc_wander.h"
#include <glm/gtx/rotate_vector.hpp>

npc_wander::npc_wander() {}

npc_wander::~npc_wander() {}

void npc_wander::initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward)
{
    m_object = object;
    m_object->set_forward(forward);
    m_object->set_position(position);
}

void npc_wander::on_update(const engine::timestep& time_step, const glm::vec3& enemy_position)
{
    float distance_to_enemy = glm::distance(m_object->position(), enemy_position);

    // check which state is the NPC in, then execute the matching behavior
    if (m_state == state::idle)
    {
        wander(time_step);

        // check whether the condition has been met to switch to the avoiding_enemy state
        if (distance_to_enemy < m_detection_radius)
            m_state = state::avoiding_enemy;
    }
    else if (m_state == state::avoiding_enemy)
    {
        avoid_enemy(time_step, enemy_position);

        // check whether the condition has been met to switch back to idle state
        if (distance_to_enemy > m_detection_radius)
            m_state = state::idle;
        // check whether the condition has been met to switch to the fleeing state
        else if (distance_to_enemy < m_trigger_radius)
            m_state = state::fleeing;
    }
    else if (m_state == state::fleeing)
    {
        flee(time_step, enemy_position);

        // check whether the condition has been met to switch back to idle state
        if (distance_to_enemy > m_detection_radius)
            m_state = state::idle;
    }
}

// move in random directions
void npc_wander::wander(const engine::timestep& time_step)
{
    m_switch_direction_timer -= (float)time_step;
    if (m_switch_direction_timer < 0.f)
    {
        float random_angle = m_random_angle(m_rng);
        glm::vec3 new_forward = glm::rotateY(m_object->forward(), random_angle);
        m_object->set_forward(new_forward);
        m_switch_direction_timer = m_default_time;
    }

    m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
    m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// turn away from the enemy
void npc_wander::avoid_enemy(const engine::timestep& time_step, const glm::vec3& enemy_position)
{
    glm::vec3 direction_away_from_enemy = m_object->position() - enemy_position;
    m_object->set_forward(direction_away_from_enemy);
    m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// move away from the enemy
void npc_wander::flee(const engine::timestep& time_step, const glm::vec3& enemy_position)
{
    glm::vec3 direction_away_from_enemy = m_object->position() - enemy_position;
    m_object->set_forward(direction_away_from_enemy);
    m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
    m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

