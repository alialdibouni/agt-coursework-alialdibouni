#include <pch.h>
#include "player.h"

player::player() : m_animation_timer(0.0f), m_speed(0.0f)
{
	;
}

player::~player()
{
}

void player::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_object->set_forward(glm::vec3(0.f, 0.f, -1.f));
	m_object->set_position(glm::vec3(0.f, 0.5, 3.f));
	m_object->animated_mesh()->set_default_animation(6);
	m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
}

void player::on_update(const engine::timestep& time_step)
{
	m_object->set_position(m_object->position() += m_object->forward() * m_speed * (float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
	// Moving The player forward and backwards
	if (engine::input::key_pressed(engine::key_codes::KEY_W))
		m_speed = 3.0f;
	else if (engine::input::key_pressed(engine::key_codes::KEY_S))
		m_speed = -3.0f;
	else
		m_speed = 0.0f;	
	// Turning the Player
	if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left
		turn(2.0f * time_step);
	else if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right
		turn(-2.0f * time_step);
	else if (engine::input::key_pressed(engine::key_codes::KEY_SPACE)) // space
		attack();

	if (m_animation_timer > 0.0f)
	{
		m_animation_timer -= (float)time_step;
		if (m_animation_timer < 0.0f)
		{
			m_object->animated_mesh()->switch_root_movement(false);
			m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
			m_speed = 1.0f;
		}
	}
	 
	m_object->animated_mesh()->on_update(time_step);
}

void player::update_camera(engine::perspective_camera& camera)
{
	float A = 10.f; //height above player	//A: 2.f (Original)    (for top down view: 10 | 3 | 6)
	float B = 3.f; //distance behind player //B 3.F (Original)
	float C = 6.f; //distance beyond player //C: 6.F (Original)

	glm::vec3 cam_pos = m_object->position() - glm::normalize(m_object->forward()) * B;
	cam_pos.y += A;

	glm::vec3 cam_look_at = m_object->position() + glm::normalize(m_object->forward()) * C;
	cam_look_at.y = 0.f;
	camera.set_view_matrix(cam_pos, cam_look_at);
}

void player::turn(float angle)
{
	m_object->set_forward(glm::rotate(m_object->forward(), angle, glm::vec3(0.f, 1.f, 0.f)));
}

void player::attack()
{
	const int animationFpsRate = 60;
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(0);
	m_speed = 0.0f;
	m_animation_timer = m_object->animated_mesh()->animations().at(0)->mDuration / animationFpsRate;
}