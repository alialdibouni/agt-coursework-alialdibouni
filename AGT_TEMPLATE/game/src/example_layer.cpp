#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

#include "pickup.h"

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())
{
    // Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::spatialised, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->play("music");
	m_audio_manager->pause("music");


	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
		(float)engine::application::window().height()));
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_mannequin_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_zombie_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);


	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp", true)
		});

	// Orc warrior from https://sketchfab.com/3d-models/orc-warrior-4f8e272891a449c8935285eab8a3a2f9
	std::vector<engine::ref<engine::texture_2d>> mannequin_textures;
	mannequin_textures.push_back(engine::texture_2d::create("assets/textures/warrior_DefaultMaterial_BaseColor.png", false));
	mannequin_textures.push_back(engine::texture_2d::create("assets/textures/weapon_DefaultMaterial_BaseColor.png", false));
	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/warrior/warrior_animation.fbx");
	m_skinned_mesh->switch_root_movement(false);
	m_skinned_mesh->set_textures(mannequin_textures);
	engine::game_object_properties mannequin_props;
	mannequin_props.animated_mesh = m_skinned_mesh;
	mannequin_props.scale = glm::vec3(0.7f);
	mannequin_props.position = glm::vec3(2.0f, 0.5f, -5.0f);
	mannequin_props.textures = mannequin_textures;
	mannequin_props.type = 0;
	mannequin_props.bounding_shape = m_skinned_mesh->size() / 2.f * mannequin_props.scale.x;
	m_mannequin = engine::game_object::create(mannequin_props);
	m_player.initialise(m_mannequin);

	// Zombie from https://sketchfab.com/3d-models/zombie-73ef58af341e46afba1da53366ed79cf
	std::vector<engine::ref<engine::texture_2d>> zombie_textures;
	zombie_textures.push_back(engine::texture_2d::create("assets/textures/zombie/Zombie.png", false));
	zombie_textures.push_back(engine::texture_2d::create("assets/textures/zombie/Zombie_ao.png", false));
	zombie_textures.push_back(engine::texture_2d::create("assets/textures/zombie/zombie_emission.png", false));
	zombie_textures.push_back(engine::texture_2d::create("assets/textures/zombie/Zombie_gloss.png", false));
	zombie_textures.push_back(engine::texture_2d::create("assets/textures/zombie/Zombie_nm.png", false));
	engine::ref<engine::skinned_mesh> m_zombie_mesh = engine::skinned_mesh::create("assets/models/animated/zombie/Zombie@Z_Idle.fbx");
	m_zombie_mesh->switch_root_movement(false);
	m_zombie_mesh->set_textures(zombie_textures);
	engine::game_object_properties zombie_props;
	zombie_props.animated_mesh = m_zombie_mesh;
	zombie_props.scale = glm::vec3(0.01f);
	zombie_props.position = glm::vec3(-1.0f, 0.5f, -2.0f);
	//zombie_props.rotation_amount = glm::radians(90.f);
	//zombie_props.rotation_axis = glm::vec3(-1.f, 0.f, 0.f);
	zombie_props.textures = zombie_textures;
	zombie_props.type = 0;
	zombie_props.bounding_shape = m_zombie_mesh->size() / 2.f * zombie_props.scale.x;	
	m_zombie = engine::game_object::create(zombie_props);

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	/*std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain.bmp",  false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);*/


	m_heightmap = engine::heightmap::create("assets/textures/heightmap.jpg", "assets/textures/Terrain.bmp", 100.f, 100.f, glm::vec3(0.f, 0.f, 0.f), 10.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { m_heightmap->mesh() };
	terrain_props.textures = { m_heightmap->texture() };
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(m_heightmap->terrain_size().x, m_physical_terrain_height, m_heightmap->terrain_size().y);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	// Load the cow model. Create a cow object. Set its properties
	engine::ref <engine::model> cow_model = engine::model::create("assets/models/static/cow4.3ds");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y, cow_model->size().z));
	cow_props.position = { -4.f,0.5f, -5.f };
	cow_props.scale = glm::vec3(cow_scale);
	cow_props.bounding_shape = cow_model->size() / 2.f * cow_scale;
	m_cow = engine::game_object::create(cow_props);

	// Load the tree model. Create a tree object. Set its properties
	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 4.f, 0.5f, -5.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);

	// Create a Sphere object, set its properties
	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);

	

	// Medkit texture from https://www.textures.com/download/manmadeboxes0007/105116
	engine::ref<engine::cuboid> pickup_shape = engine::cuboid::create(glm::vec3(0.5f), false);
	engine::ref<engine::texture_2d> pickup_texture = engine::texture_2d::create("assets/textures/medkit.jpg", true);
	engine::game_object_properties pickup_props;
	pickup_props.position = { 5.f, 1.f, 5.f };
	pickup_props.meshes = { pickup_shape->mesh() };
	pickup_props.textures = { pickup_texture };
	m_pickup = pickup::create(pickup_props);
	m_pickup->init();

	// Create a 3d pentagon object, set its properties
	engine::ref<engine::pentagon> pentagon_shape = engine::pentagon::create(0.5f, 0.2f);
	engine::game_object_properties pentagon_props;
	pentagon_props.position = { 0.f, 1.f, 0.f };
	pentagon_props.meshes = { pentagon_shape->mesh() };
	m_pentagon = engine::game_object::create(pentagon_props);
	m_pentagon_pickup = pickup::create(pentagon_props);
	m_pentagon_pickup->init();

	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_ball);
	//m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_skinned_mesh->switch_animation(6);
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step)
{

	if (m_show_menu) {
		// When Enter is pressed, remove the menu screen
		if (engine::input::key_pressed(engine::key_codes::KEY_ENTER)) {
			m_show_menu = false;
		}
		return; // Skip the rest of the update if the menu is active
	}

    m_3d_camera.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	//m_mannequin->animated_mesh()->on_update(time_step);
	m_player.on_update(time_step);
	m_mannequin->animated_mesh()->on_update(time_step);
	m_player.update_camera(m_3d_camera);

	m_zombie->animated_mesh()->on_update(time_step);

	m_audio_manager->update_with_camera(m_3d_camera);

	m_pickup->update(m_player.position(), time_step);

	// Update the pentagon pickup
	m_pentagon_pickup->update(m_player.position(), time_step);

	check_bounce();

	update_ground_positions();
} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();

	if (m_show_menu) {
		// Render the menu background
		engine::render_command::clear_color({ 0.0f, 0.0f, 0.0f, 1.0f });
		engine::render_command::clear();

		// Render the menu text
		const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
		m_text_manager->render_text(text_shader, "Zombie Defence",
			engine::application::window().width() / 2.0f - 200.0f,
			engine::application::window().height() / 2.0f,
			1.0f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		m_text_manager->render_text(text_shader, "Press Enter to Start",
			engine::application::window().width() / 2.0f - 150.0f,
			engine::application::window().height() / 2.0f - 50.0f,
			0.5f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		return; // Skip the rest of the rendering if the menu is active
	}

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(mesh_shader, m_terrain);

	//Render the Tree object
	glm::mat4 tree_transform(1.0f);
	tree_transform = glm::translate(tree_transform, glm::vec3(4.f, 0.5, -5.0f));
	tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	tree_transform = glm::scale(tree_transform, m_tree->scale());
	engine::renderer::submit(mesh_shader, tree_transform, m_tree);
	//Render a row of trees using a loop
	for (int i = 0; i < 5; i++)
	{
		tree_transform = glm::translate(tree_transform, glm::vec3(0.f, 0.f, -2.f));
		engine::renderer::submit(mesh_shader, tree_transform, m_tree);
	}
	
	//Render the Cow object
	glm::mat4 cow_transform(1.0f);
	cow_transform = glm::translate(cow_transform, m_cow->position()); //Cow's position
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis()); //Cow's rotation
	//cow_transform = glm::rotate(cow_transform, glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f)); //Cow's new rotation by 45 degrees on y axis
	cow_transform = glm::scale(cow_transform, m_cow->scale()); //Cow's scale
	engine::renderer::submit(mesh_shader, cow_transform, m_cow);
	
	//Render another cow object
	cow_transform = glm::translate(cow_transform, glm::vec3(0.f, 1.5f, 0.f)); //cow new position
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis()); //Cow's rotation
	cow_transform = glm::scale(cow_transform, glm::vec3(2.f, 2.f, 2.f)); //Cow's new scale
	engine::renderer::submit(mesh_shader, cow_transform, m_cow); //cow 2
	
	//Render another cow object
	cow_transform = glm::translate(cow_transform, glm::vec3(0.f, 3.5f, 0.f)); //cow new position
	cow_transform = glm::rotate(cow_transform, glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f)); //Cow's new rotation by 45 degrees on y axis
	engine::renderer::submit(mesh_shader, cow_transform, m_cow); //cow 3

	//Rendering a cow that looks at the camera
	glm::mat4 cow_transform2(1.0f);
	glm::vec3 p = glm::vec3(0.f, 0.5f, 5.f);
	glm::vec3 c = m_3d_camera.position();
	glm::vec3 v = c - p;
	float theta = atan2(v.x, v.z);
	cow_transform2 = glm::translate(cow_transform2, p);
	cow_transform2 = glm::rotate(cow_transform2, theta, glm::vec3(0.f, 1.f, 0.f));
	cow_transform2 = glm::scale(cow_transform2, m_cow->scale());
	engine::renderer::submit(mesh_shader, cow_transform2, m_cow);
	
	//Render the 3d pentagon object
	if (m_pentagon_pickup->active()) {
		glm::mat4 pentagon_pickup_transform(1.0f);
		pentagon_pickup_transform = glm::translate(pentagon_pickup_transform, m_pentagon_pickup->position());
		pentagon_pickup_transform = glm::rotate(pentagon_pickup_transform, m_pentagon_pickup->rotation_amount(), m_pentagon_pickup->rotation_axis());
		//engine::renderer::submit(mesh_shader, m_pentagon);
		engine::renderer::submit(mesh_shader, m_pentagon->meshes().at(0), pentagon_pickup_transform);

	}

	//Render the pickup object
	if (m_pickup->active()) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);
		m_pickup->textures().at(0)->bind();
		glm::mat4 pickup_transform(1.0f);
		pickup_transform = glm::translate(pickup_transform, m_pickup->position());
		pickup_transform = glm::rotate(pickup_transform, m_pickup->rotation_amount(), m_pickup->rotation_axis());
		engine::renderer::submit(mesh_shader, m_pickup->meshes().at(0), pickup_transform);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", false);
	}

	m_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_ball);

	m_mannequin_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_player.object());
	//engine::renderer::submit(mesh_shader, m_mannequin);

	// Render the zombie
	m_zombie_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_zombie);

    engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height()-25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }
    } 
}

void example_layer::check_bounce()
{
	if (m_prev_sphere_y_vel < 0.1f && m_ball->velocity().y > 0.1f)
		//m_audio_manager->play("bounce");
		m_audio_manager->play_spatialised_sound("bounce", m_3d_camera.position(), glm::vec3(m_ball->position().x, 0.f, m_ball->position().z));
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}

void example_layer::update_ground_positions()
{
	m_mannequin->set_position(glm::vec3(m_mannequin->position().x, m_heightmap->ground_height(m_mannequin->position()), m_mannequin->position().z));
	m_zombie->set_position(glm::vec3(m_zombie->position().x, m_heightmap->ground_height(m_zombie->position()), m_zombie->position().z));
	m_cow->set_position(glm::vec3(m_cow->position().x, m_heightmap->ground_height(m_cow->position()), m_cow->position().z));
	m_tree->set_position(glm::vec3(m_tree->position().x, m_heightmap->ground_height(m_tree->position()), m_tree->position().z));
	m_pentagon_pickup->set_position(glm::vec3(m_pentagon_pickup->position().x, m_heightmap->ground_height(m_pentagon_pickup->position()), m_pentagon_pickup->position().z));
	m_pickup->set_position(glm::vec3(m_pickup->position().x, m_heightmap->ground_height(m_pickup->position()), m_pickup->position().z));
}
