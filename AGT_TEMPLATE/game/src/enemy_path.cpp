// = The code written has been assisted with Github Copilot AI = //

#include "pch.h"
#include "enemy_path.h"

enemy_path::enemy_path() : m_current_path_index(0), m_speed(1.0f), m_detection_radius(5.0f), m_trigger_radius(2.0f), m_state(state::idle) {}

enemy_path::~enemy_path() {}

void enemy_path::initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward, const std::vector<glm::vec3>& path)
{
    m_object = object;
    m_object->set_forward(forward);
    m_object->set_position(position);
    m_path = path;
    m_current_path_index = 0; // Initialize the current path index
}

void enemy_path::on_update(const engine::timestep& time_step, const glm::vec3& player_position)
{
    float distance_to_player = glm::distance(m_object->position(), player_position);

    // check which state is the enemy in, then execute the matching behaviour
    if (m_state == state::idle)
    {
        pathfind(time_step);

        // check whether the condition has been met to switch to the on_guard state
        if (distance_to_player < m_detection_radius)
            m_state = state::on_guard;
    }
    else if (m_state == state::on_guard)
    {
        face_player(time_step, player_position);

        // check whether the condition has been met to switch back to idle state
        if (distance_to_player > m_detection_radius)
            m_state = state::idle;
        // check whether the condition has been met to switch to the chasing state
        else if (distance_to_player < m_trigger_radius)
            m_state = state::chasing;
    }
    else if (m_state == state::chasing)
    {
        chase_player(time_step, player_position);

        // check whether the condition has been met to switch back to idle state
        if (distance_to_player > m_detection_radius)
            m_state = state::idle;
    }
    else if (m_state == state::pathfinding)
    {
        pathfind(time_step);
    }
}

// follow a set path
void enemy_path::pathfind(const engine::timestep& time_step)
{
    if (m_path.empty())
        return;

    glm::vec3 target_position = m_path[m_current_path_index];
    glm::vec3 direction = target_position - m_object->position();
    if (glm::length(direction) < 0.1f)
    {
        m_current_path_index = (m_current_path_index + 1) % m_path.size();
        target_position = m_path[m_current_path_index];
        direction = target_position - m_object->position();
    }

    m_object->set_forward(glm::normalize(direction));
    m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
    m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// stop and look at the player
void enemy_path::face_player(const engine::timestep& time_step, const glm::vec3& player_position)
{
    m_object->set_forward(player_position - m_object->position());
    m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// move forwards in the direction of the player
void enemy_path::chase_player(const engine::timestep& time_step, const glm::vec3& player_position)
{
    m_object->set_forward(player_position - m_object->position());
    m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
    m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}
