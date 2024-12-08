#include "pch.h"
#include "projectile.h"
#include "player.h"

projectile::projectile()
{}

projectile::~projectile()
{}

void projectile::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_rotational_inertia = (2.f / 3.f) * object->mass() * (m_object->bounding_shape().y / 2.f) * (m_object->bounding_shape().y / 2.f);
	m_contact_time = 0.05f;
}

void projectile::toss(const player& player, float toss)
{
	//m_object->set_velocity(glm::vec3(0.f));
	glm::vec3 force = player.object()->forward() * toss;
	m_instantaneous_acceleration = force / m_object->mass();

	m_object->set_acceleration(glm::vec3(0.f, -9.8f, 0.f));
	m_object->set_torque(glm::vec3(0.f));

	glm::vec3 torque = glm::vec3(1, 0, 0);
	torque *= toss / 90.0f;
	m_instantaneous_angular_acceleration = torque / m_rotational_inertia;

	m_object->set_rotation_amount(0.f);
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_angular_velocity(glm::vec3(0.f));
	m_contact_time = 0.0f;

	// Set the ball to the current camera position
	m_object->set_position(player.position());
	m_object->set_velocity(25.0f * player.object()->forward());

	// Determine rotation angles of camera (from Lab 4)
	m_theta = engine::PI / 2.f - acos(player.object()->forward().y);
	m_phi = atan2(player.object()->forward().x, player.object()->forward().z);
}

void projectile::on_update(const engine::timestep& time_step)
{
	// Update physical quanitities
	m_last_position = m_object->position();

	m_object->set_position(m_object->position() + m_object->velocity() * (float)time_step);
	m_object->set_velocity(m_object->velocity() + (m_object->acceleration() + m_instantaneous_acceleration) * (float)time_step);

	glm::vec3 angle = m_object->rotation_axis() * m_object->rotation_amount();
	angle += m_object->angular_velocity() * (float)time_step;
	m_object->set_rotation_amount(glm::length(angle));
	if (glm::length(angle) > 0.f)
	{
		m_object->set_rotation_axis(glm::normalize(angle));
	}

	m_object->set_angular_velocity(m_object->angular_velocity() + (m_object->torque() + m_instantaneous_angular_acceleration) * (float)time_step);

	// Turn off instantaneous forces if contact time is surpassed
	if (glm::length(m_instantaneous_acceleration) > 0 && m_contact_time > 0.05) {
		m_instantaneous_acceleration = glm::vec3(0.f);
		m_instantaneous_angular_acceleration = glm::vec3(0.f);
		m_contact_time = 0.f;
	}
	m_contact_time += time_step;

	// Check for collision detection
	float y_plane = 0.5f;
	if (collision_detection(y_plane)) {
		collision_response(y_plane);
	}
}

void projectile::on_render(const engine::ref<engine::shader>& shader)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_object->position() + glm::vec3(0.f, 1.5f, 0.f));
	transform = glm::rotate(transform, -m_theta, glm::vec3(1.f, 0.f, 0.f));
	transform = glm::rotate(transform, m_phi, glm::vec3(0.f, 1.f, 0.f));
	transform = glm::rotate(transform, m_object->rotation_amount(), m_object->rotation_axis());
	transform = glm::scale(transform, m_object->scale());
	engine::renderer::submit(shader, transform, m_object);
}

bool projectile::collision_detection(float y_plane)
{
	// Check for collision with the ground by looking at the y value of the ball's position
	if (m_object->position().y - m_object->bounding_shape().y < y_plane && m_object->velocity().y < 0) {
		return true;
	}
	return false;
}

void projectile::collision_response(float y_plane)
{
	float convergenceThreshold = 0.5f;
	if (glm::length(m_object->velocity()) > convergenceThreshold) {
		// The ball has bounced!  Implement a bounce by flipping the y velocity
		m_object->set_velocity(glm::vec3(m_object->velocity().x, -m_object->velocity().y, m_object->velocity().z));

		m_object->set_velocity(m_object->velocity() * m_object->restitution());
		m_object->set_angular_velocity(m_object->angular_velocity() * m_object->restitution());
	}
	else {
		// Velocity of the ball is below a threshold.  Stop the ball. 
		m_object->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
		m_object->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
		m_object->set_position(glm::vec3(m_object->position().x, m_object->bounding_shape().y + y_plane, m_object->position().z));
		m_object->set_angular_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}
