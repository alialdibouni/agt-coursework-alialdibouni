#pragma once
#include <engine.h>
#include "player.h"
#include "projectile.h"
#include <engine/entities/shapes/heightmap.h>
#include "engine/entities/bounding_box.h"
#include "enemy.h"
#include "enemy_path.h"
#include "cross_fade.h"
#include "billboard.h"
#include "npc_wander.h"

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
	bool								m_game_over = false;

	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_cow{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_ball{};
	engine::ref<engine::game_object>	m_mannequin{};
	engine::ref<engine::game_object>	m_zombie{};   // mesh based object
	engine::ref<engine::game_object>	m_zombie2{};   // mesh based object
	engine::ref<engine::game_object>	m_civilian{}; // mesh based object
	engine::ref<engine::game_object>    m_dog{};	  // mesh based object
	engine::ref<engine::game_object>    m_pentagon{}; //primitive opengl object
	engine::ref<engine::game_object>    m_cylinder{}; //primitive opengl object
	engine::ref<engine::game_object>    m_heart{};	  //primitive opengl object
;	engine::ref<engine::game_object>    m_cone{};	  //primitive opengl object
	
	//pickups
	engine::ref<pickup>					m_pickup{};
	engine::ref<pickup>					m_pentagon_pickup{};
	engine::ref<pickup>					m_heart_pickup{};
	engine::ref<pickup>					m_cone_pickup{};

	player								m_player{};
	enemy								m_enemy{};
	enemy_path							m_enemy_path{};
	npc_wander							m_npc_wander{};

	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_heart_material{};
	engine::ref<engine::material>		m_lightsource_material{};
	engine::ref<engine::material>		m_lightsource_material2{};
	engine::ref<engine::material>		m_spotlightsource_material{};
	engine::ref<engine::material>		m_mannequin_material{};
	engine::ref<engine::material>		m_zombie_material{};
	engine::ref<engine::material>		m_zombie2_material{};
	engine::ref<engine::material>		m_civilian_material{};

	engine::bounding_box				m_player_box{};
	engine::bounding_box				m_zombie_box{};
	engine::bounding_box				m_zombie2_box{};
	engine::bounding_box				m_grenade_box{};

	engine::DirectionalLight            m_directionalLight;
	engine::PointLight					m_pointLight;
	engine::PointLight					m_pointLight2;
	uint32_t							num_point_lights = 2;
	engine::SpotLight					m_spotLight;
	uint32_t							num_spot_lights = 1;


	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

    engine::orthographic_camera       m_2d_camera; 
    engine::perspective_camera        m_3d_camera;

	engine::ref<engine::heightmap>	  m_heightmap;

	//effects
	engine::ref<cross_fade>							m_cross_fade{};
	engine::ref<billboard>							m_billboard{};

	float							  m_physical_terrain_height = 0.5f;

	projectile m_projectile;
	int m_score{ 0 };
	int m_grenades{ 3 };
	float m_timer = 0.0f;
	bool m_pentagon_pickup_collected{ false };
	bool m_heart_pickup_collected{ false };
	bool m_grenade_pickup_collected{ false };
	float m_health_cooldown = 0.0f;
	bool m_fog = false;
};
