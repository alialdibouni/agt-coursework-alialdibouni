#pragma once
#include <engine.h>
#include "player.h"
#include <engine/entities/shapes/heightmap.h>

class pickup;

class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();

    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:
	void check_bounce();
	void update_ground_positions();

	bool								m_show_menu = true;

	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_cow{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_ball{};
	engine::ref<engine::game_object>	m_mannequin{};
	engine::ref<engine::game_object>	m_zombie{};
	engine::ref<pickup> m_pickup{};
	engine::ref<engine::game_object>    m_pentagon{};
	engine::ref<pickup> m_pentagon_pickup{};
	player								m_player{};

	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_mannequin_material{};
	engine::ref<engine::material>		m_zombie_material{};

	engine::DirectionalLight            m_directionalLight;

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

    engine::orthographic_camera       m_2d_camera; 
    engine::perspective_camera        m_3d_camera;

	engine::ref<engine::heightmap>	  m_heightmap;

	float							  m_physical_terrain_height = 0.5f;
};
