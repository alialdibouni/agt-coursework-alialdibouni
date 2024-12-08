// = The code written has been assisted with Github Copilot AI = //

#pragma once
#include <engine.h>
#include <random>

class npc_wander
{
    enum class state
    {
        idle,
        wandering,
        avoiding_enemy,
        fleeing
    };
public:
    npc_wander();
    ~npc_wander();
    // set the parameters for the NPC
    void initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward);
    // update the NPC
    void on_update(const engine::timestep& time_step, const glm::vec3& enemy_position);

    // methods controlling the NPC’s behavior in a certain state
    void wander(const engine::timestep& time_step);
    void avoid_enemy(const engine::timestep& time_step, const glm::vec3& enemy_position);
    void flee(const engine::timestep& time_step, const glm::vec3& enemy_position);
    // game object bound to the NPC
    engine::ref<engine::game_object> object() const { return m_object; }
private:
    // NPC's speed
    float m_speed{ 0.5f };
    // timer controlling the direction switch and the reset value for this timer
    float m_default_time{ 4.f };
    float m_switch_direction_timer = m_default_time;
    // threshold distances
    float m_detection_radius{ 6.f };
    float m_trigger_radius{ 3.f };
    // game object bound to the NPC
    engine::ref<engine::game_object> m_object;
    // current state of the NPC's state machine
    state m_state = state::idle;
    // random number generator for wandering behavior
    std::mt19937 m_rng{ std::random_device{}() };
    std::uniform_real_distribution<float> m_random_angle{ -glm::pi<float>(), glm::pi<float>() };
};

