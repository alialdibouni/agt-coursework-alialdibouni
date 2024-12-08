// = The code written has been assisted with Github Copilot AI = //

#pragma once
#include <engine.h>
#include <vector>

class enemy_path
{
	enum class state
	{
		idle,
		on_guard,
		chasing,
		pathfinding
	};
public:
	enemy_path();
	~enemy_path();
	// set the parameters for the enemy
	void initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward, const std::vector<glm::vec3>& path);
	// update the enemy
	void on_update(const engine::timestep& time_step, const glm::vec3& player_position);

	// methods controlling the enemy’s behaviour in a certain state
	void pathfind(const engine::timestep& time_step);
	void face_player(const engine::timestep& time_step, const glm::vec3& player_position);
	void chase_player(const engine::timestep& time_step, const glm::vec3& player_position);
	// game object bound to the enemy
	engine::ref<engine::game_object> object() const { return m_object; }
private:
	// enemy's speed
	float m_speed{ 2.5f };
	// pathfinding properties
	std::vector<glm::vec3> m_path;
	size_t m_current_path_index{ 0 };
	// threshold distances
	float m_detection_radius{ 6.f };
	float m_trigger_radius{ 3.f };
	// game object bound to the enemy
	engine::ref< engine::game_object> m_object;
	//current state of the enemy's state machine
	state m_state = state::idle;
};

