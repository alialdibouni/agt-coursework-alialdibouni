#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

#include "pickup.h"
#include <engine/entities/shapes/cylinder.h>
#include <engine/entities/shapes/heart.h>
#include <engine/entities/shapes/cone.h>

example_layer::example_layer() 
    :m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f), 
    m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height()),
	m_timer(0.0f)
{
	m_fog = false;
	glm::vec3 m_fog_colour = glm::vec3(0.5f);
	int m_fog_factor_type = 2;

	// Hide the mouse and lock it inside the window
    //engine::input::anchor_mouse(true);
    engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::spatialised, "bounce"); // Royalty free sound from freesound.org
	//m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->load_sound("assets/audio/spooky-ambiance.mp3", engine::sound_type::track, "background");  // from https://pixabay.com/users/freesound_community-46691455
	//m_audio_manager->play("music");
	//m_audio_manager->pause("music");
	m_audio_manager->load_sound("assets/audio/coin_collected.mp3", engine::sound_type::event, "coin");
	m_audio_manager->load_sound("assets/audio/heartbeat.mp3", engine::sound_type::event, "heart");
	m_audio_manager->load_sound("assets/audio/grenade.mp3", engine::sound_type::event, "grenade");
	m_audio_manager->load_sound("assets/audio/reload.mp3", engine::sound_type::event, "reload");


	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	//night time directional light (for spotlight)
	m_directionalLight.Color = glm::vec3(0.3f, 0.3f, 0.3f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	//point light properties
	m_pointLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_pointLight.AmbientIntensity = 0.125f;
	m_pointLight.DiffuseIntensity = 0.3f;
	m_pointLight.Position = glm::vec3(0.0f, 2.0f, 0.0f);

	m_pointLight2.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_pointLight2.AmbientIntensity = 0.125f;
	m_pointLight2.DiffuseIntensity = 0.3f;
	m_pointLight2.Position = glm::vec3(0.0f, 2.0f, 0.0f);

	//spot light properties
	m_spotLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_spotLight.AmbientIntensity = 1.f;
	m_spotLight.DiffuseIntensity = 0.6f;
	m_spotLight.Position = glm::vec3(-5.0f, 2.0f, 0.0f);
	m_spotLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
	m_spotLight.Cutoff = 0.5f;
	m_spotLight.Attenuation.Constant = 1.0f;
	m_spotLight.Attenuation.Linear = 0.1f;
	m_spotLight.Attenuation.Exp = 0.01f;

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

	m_heart_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_mannequin_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_zombie_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_zombie2_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_civilian_material = engine::material::create(1.0f, glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_lightsource_material = engine::material::create(1.0f, m_pointLight.Color,
		m_pointLight.Color, m_pointLight.Color, 1.0f);

	m_lightsource_material2 = engine::material::create(1.0f, m_pointLight2.Color,
		m_pointLight2.Color, m_pointLight2.Color, 1.0f);

	m_spotlightsource_material = engine::material::create(1.0f, m_spotLight.Color,
		m_spotLight.Color, m_spotLight.Color, 1.0f);

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
	mannequin_props.bounding_shape = glm::vec3(m_skinned_mesh->size().x / 8.f,	m_skinned_mesh->size().y / 3.8f,	m_skinned_mesh->size().z / 12.f);
	m_mannequin = engine::game_object::create(mannequin_props);
	m_player.initialise(m_mannequin);
	m_player_box.set_box(mannequin_props.bounding_shape.x* mannequin_props.scale.x,
		mannequin_props.bounding_shape.y* mannequin_props.scale.y,
		mannequin_props.bounding_shape.z* mannequin_props.scale.z,
		mannequin_props.position);


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
	zombie_props.position = glm::vec3(-2.0f, 0.5f, -4.0f);
	zombie_props.textures = zombie_textures;
	zombie_props.type = 0;
	zombie_props.bounding_shape = glm::vec3(m_skinned_mesh->size().x * 5.f, m_skinned_mesh->size().y * 25.f, m_skinned_mesh->size().z * 2.5f);
	m_zombie = engine::game_object::create(zombie_props);
	m_zombie_box.set_box(zombie_props.bounding_shape.x * zombie_props.scale.x,
	zombie_props.bounding_shape.y * zombie_props.scale.y,
	zombie_props.bounding_shape.z * zombie_props.scale.z,
	zombie_props.position);
	m_enemy.initialise(m_zombie, zombie_props.position, glm::vec3(1.f, 0.f, 0.f));

	// zombie2 from https://sketchfab.com/3d-models/6139xxxxx-zombie-zombie-walk-9aa7d48ddacc4326b3450d9d624b6dfe
	std::vector<engine::ref<engine::texture_2d>> zombie2_textures;
	zombie2_textures.push_back(engine::texture_2d::create("assets/textures/zombie2/Zombie_diffuse.png", false));
	zombie2_textures.push_back(engine::texture_2d::create("assets/textures/zombie2/Zombie_normal.png", false));
	zombie2_textures.push_back(engine::texture_2d::create("assets/textures/zombie2/Zombie_specular.png", false));
	engine::ref<engine::skinned_mesh> m_zombie2_mesh = engine::skinned_mesh::create("assets/models/animated/zombie2/Zombie_Walk.fbx");
	m_zombie2_mesh->switch_root_movement(false);
	m_zombie2_mesh->set_textures(zombie2_textures);
	engine::game_object_properties zombie2_props;
	zombie2_props.animated_mesh = m_zombie2_mesh;
	zombie2_props.scale = glm::vec3(1.f);
	zombie2_props.position = glm::vec3(-8.0f, 1.f, 5.0f);
	zombie2_props.textures = zombie2_textures;
	zombie2_props.type = 0;
	zombie2_props.bounding_shape = glm::vec3(m_zombie2_mesh->size().x / 200.f, m_zombie2_mesh->size().y / 100.f, m_zombie2_mesh->size().z / 45.f);
	m_zombie2 = engine::game_object::create(zombie2_props);
	m_zombie2_box.set_box(zombie2_props.bounding_shape.x* zombie2_props.scale.x,
		zombie2_props.bounding_shape.y* zombie2_props.scale.y,
		zombie2_props.bounding_shape.z* zombie2_props.scale.z,
		zombie2_props.position);
	std::vector<glm::vec3> path = {
		{ 1.f, 0.0f, 1.f },
		{ 5.f, 0.0f, 5.f }
	};
	m_enemy_path.initialise(m_zombie2, zombie2_props.position, glm::vec3(1.f, 0.f, 0.f), path);
	
	// woman from https://sketchfab.com/3d-models/female-jogging-free-animation-30-frames-loop-677ed671c45c4de199ef3deeaee0bed9
	std::vector<engine::ref<engine::texture_2d>> woman_textures;
	woman_textures.push_back(engine::texture_2d::create("assets/textures/woman/peopleColors.png", false));
	engine::ref<engine::skinned_mesh> woman_mesh = engine::skinned_mesh::create("assets/models/animated/woman/woman_jog_maya.fbx");
	woman_mesh->switch_root_movement(false);
	woman_mesh->set_textures(woman_textures);
	engine::game_object_properties woman_props;
	woman_props.animated_mesh = woman_mesh;
	woman_props.scale = glm::vec3(1.f);
	woman_props.position = glm::vec3(8.f, 0.f, -5.f);
	woman_props.textures = woman_textures;
	woman_props.type = 0;
	woman_props.bounding_shape = glm::vec3(woman_mesh->size() / 2.f);
	m_civilian = engine::game_object::create(woman_props);
	m_npc_wander.initialise(m_civilian, woman_props.position, glm::vec3(1.f, 0.f, 0.f));

	// Heightmap generated from https://manticorp.github.io/unrealheightmap/index.html#latitude/27.99440141104617/longitude/86.923828125/zoom/11/outputzoom/14/width/512/height/512 (Not actual location)
	// Terrrain texture from https://3designer.app/textures/seamless/dirt-ground
	m_heightmap = engine::heightmap::create("assets/textures/heightmap.jpg", "assets/textures/dirttexture.jpeg", 100.f, 100.f, glm::vec3(0.f, 0.f, 0.f), 10.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { m_heightmap->mesh() };
	terrain_props.textures = { m_heightmap->texture() };
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(m_heightmap->terrain_size().x, m_physical_terrain_height, m_heightmap->terrain_size().y);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	//football properties from lab 6
	engine::ref <engine::model> sphere_shape = engine::model::create("assets/models/static/Grenade.fbx");
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = sphere_shape->meshes();
	sphere_props.textures = sphere_shape->textures();
	sphere_props.type = 1;
	sphere_props.bounding_shape = sphere_shape->size() / 2.0f;
	sphere_props.scale = glm::vec3(0.3f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.18f; //weight of a grenade 180g
	m_ball = engine::game_object::create(sphere_props);
	m_grenade_box.set_box(sphere_props.bounding_shape.x, sphere_props.bounding_shape.y, sphere_props.bounding_shape.z, sphere_props.position);

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
	engine::ref<engine::texture_2d> pentagon_texture = engine::texture_2d::create("assets/textures/gold.jpg", true);
	engine::game_object_properties pentagon_props;
	pentagon_props.position = { 0.f, 1.f, 0.f };
	pentagon_props.meshes = { pentagon_shape->mesh() };
	pentagon_props.textures = { pentagon_texture };
	m_pentagon = engine::game_object::create(pentagon_props);
	m_pentagon_pickup = pickup::create(pentagon_props);
	m_pentagon_pickup->init();

	// Create a cylinder object, set its properties
	engine::game_object_properties cylinder_props;
	cylinder_props.position = { -5.0f, 10.0f, 5.0f };
	cylinder_props.scale = { 1.0f, 1.0f, 1.0f };
	cylinder_props.meshes = { engine::cylinder::create(1.0f, 2.0f, 32)->mesh() };
	m_cylinder = engine::game_object::create(cylinder_props);

	// Create a heart object, set its properties
	engine::ref<engine::heart> heart_shape = engine::heart::create(0.03f, 0.2f); // Size and depth of the heart
	engine::ref<engine::texture_2d> heart_texture = engine::texture_2d::create("assets/textures/red.jpg", true);
	engine::game_object_properties heart_props;
	heart_props.position = { 0.0f, 1.f, -3.0f };
	heart_props.meshes = { heart_shape->mesh()};
	heart_props.textures = { heart_texture };
	m_heart = engine::game_object::create(heart_props);
	m_heart_pickup = pickup::create(heart_props);
	m_heart_pickup->init();

	// Create a cone object, set its properties
	engine::ref<engine::texture_2d> cone_texture = engine::texture_2d::create("assets/textures/blue.jpg", true);
	engine::game_object_properties cone_props;
	cone_props.position = { 3.0f, 1.0f, 2.0f };
	cone_props.scale = { 0.5f, 0.5f, 0.5f };
	cone_props.meshes = { engine::cone::create(1.0f, 2.0f, 32)->mesh() };
	cone_props.textures = { cone_texture };
	m_cone = engine::game_object::create(cone_props);
	m_cone_pickup = pickup::create(cone_props);
	m_cone_pickup->init();

	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_ball);
	//m_game_objects.push_back(m_pickup);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_skinned_mesh->switch_animation(6);

	//effects
	m_cross_fade = cross_fade::create("assets/textures/Red.bmp", .5f, 1.6f, 0.9f);
	m_billboard = billboard::create("assets/textures/Explosion.tga", 4, 5, 16);

	m_projectile.initialise(m_ball);
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step)
{

	if (m_show_menu) {
		// When Enter is pressed, remove the menu screen
		if (engine::input::key_pressed(engine::key_codes::KEY_ENTER)) {
			m_show_menu = false;
			m_audio_manager->play("background");
		}
		return; // Skip the rest of the update if the menu is active
	}

	//Game Over Screen
	if (m_player.health() <= 0) {
		m_game_over = true; //show game over screen
		m_audio_manager->pause("background");
		/*if (engine::input::key_pressed(engine::key_codes::KEY_R)) {
			m_game_over = false;
			m_audio_manager->play("background");
			m_player.set_health(3);
			m_score = 0;
		}*/
		return;
	}
		
	glm::vec3 pos = m_player.object()->position();
	glm::vec3 enemy_pos = m_enemy.object()->position();
	glm::vec3 enemy2_pos = m_enemy_path.object()->position();
	glm::vec3 grenade_pos = m_projectile.object()->position();

    m_3d_camera.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	m_player.on_update(time_step);
	m_mannequin->animated_mesh()->on_update(time_step);
	m_player_box.on_update(m_player.object()->position());

	m_player.update_camera(m_3d_camera);

	m_enemy.on_update(time_step, m_player.object()->position());
	m_zombie->animated_mesh()->on_update(time_step);
	m_zombie_box.on_update(m_enemy.object()->position());

	m_enemy_path.on_update(time_step, m_player.object()->position());
	m_zombie2->animated_mesh()->on_update(time_step);
	m_zombie2_box.on_update(m_enemy_path.object()->position());

	
	m_civilian->animated_mesh()->on_update(time_step);
	m_npc_wander.on_update(time_step, m_enemy.object()->position());
	m_npc_wander.on_update(time_step, m_enemy_path.object()->position());

	if (m_health_cooldown > 0.0f)
		m_health_cooldown -= (float)time_step;

	if (m_zombie_box.collision(m_player_box))
	{
		m_player.object()->set_position(pos);
		m_enemy.object()->set_position(enemy_pos);
	}
	if (m_zombie2_box.collision(m_player_box))
	{
		m_player.object()->set_position(pos);
		m_enemy_path.object()->set_position(enemy2_pos);
	}

	//needs changing / altering
	if (m_zombie_box.collision(m_grenade_box) || m_zombie2_box.collision(m_grenade_box))
	{
		//remove m_zombie object
		m_billboard->activate(m_ball->position() + glm::vec3(0.f, 1.f, 0.f), 4.f, 4.f);
		m_audio_manager->play("grenade");
		m_score++;
		m_enemy.object()->set_position(glm::vec3(-100.f, 0.f, -100.f)); //need to remove zombie object
	}
	if (m_zombie2_box.collision(m_grenade_box))
	{
		//remove m_zombie object
		m_billboard->activate(m_ball->position() + glm::vec3(0.f, 1.f, 0.f), 4.f, 4.f);
		m_audio_manager->play("grenade");
		m_score++;
		m_enemy_path.object()->set_position(glm::vec3(-100.f, 0.f, -100.f)); //need to remove zombie object
	}

	m_cross_fade->on_update(time_step);
	//show red screen to indicate getting hit?
	if (m_zombie_box.collision(m_player_box) && m_health_cooldown <=0.0f) {
		int health = m_player.health();
		m_cross_fade->activate();
		m_player.set_health(m_player.health() - 1);
		m_health_cooldown = 1.0f;
	}
	if (m_zombie2_box.collision(m_player_box) && m_health_cooldown <= 0.0f) {
		int health = m_player.health();
		m_cross_fade->activate();
		m_player.set_health(m_player.health() - 1);
		m_health_cooldown = 1.0f;
	}

	m_billboard->on_update(time_step);

	m_projectile.on_update(time_step);
	m_grenade_box.on_update(m_ball->position());

	m_audio_manager->update_with_camera(m_3d_camera);

	m_pickup->update(m_player.position(), time_step);

	// Update the pentagon pickup
	m_pentagon_pickup->update(m_player.position(), time_step);

	//update heart pickup
	m_heart_pickup->update(m_player.position(), time_step);	

	m_cone_pickup->update(m_player.position(), time_step);

	//pick up pentagon 
	if (!m_pentagon_pickup->active() && m_pentagon_pickup_collected == false)
	{
		m_score++;
		m_pentagon_pickup_collected = true;
		m_audio_manager->play("coin");
		m_pointLight.Color = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	//pick up heart
	if (!m_heart_pickup->active() && m_heart_pickup_collected == false)
	{
		m_player.set_health(m_player.health() + 1);
		m_heart_pickup_collected = true;
		m_audio_manager->play("heart");
		m_pointLight2.Color = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	if (!m_cone_pickup->active() && m_grenade_pickup_collected == false) {
		m_grenades = m_grenades + 5;
		m_grenade_pickup_collected = true;
		m_audio_manager->play("reload");
	}

	m_timer += time_step.seconds();

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
		m_text_manager->render_text(text_shader, "Zombie Survival",
			engine::application::window().width() / 2.0f - 200.0f,
			engine::application::window().height() / 2.f,
			1.0f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		m_text_manager->render_text(text_shader, "Press Enter to Start",
			engine::application::window().width() / 2.0f - 150.0f,
			engine::application::window().height() / 2.0f - 50.0f,
			0.5f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		m_text_manager->render_text(text_shader, "Controls",
			engine::application::window().width() / 2.0f - 600.0f,
			engine::application::window().height() / 2.0f - 50.0f,
			0.5f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		m_text_manager->render_text(text_shader, "W & S: Move Player Forward / Backwards",
			engine::application::window().width() / 2.0f - 600.0f,
			engine::application::window().height() / 2.0f - 70.0f,
			0.3f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		m_text_manager->render_text(text_shader, "A & D: Turn Player Left / Right",
			engine::application::window().width() / 2.0f - 600.0f,
			engine::application::window().height() / 2.0f - 90.0f,
			0.3f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		m_text_manager->render_text(text_shader, "G: Throw Grenade",
			engine::application::window().width() / 2.0f - 600.0f,
			engine::application::window().height() / 2.0f - 110.0f,
			0.3f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		return; // Skip the rest of the rendering if the menu is active
	}

	if (m_game_over) {
		// Render the menu background
		engine::render_command::clear_color({ 0.0f, 0.0f, 0.0f, 1.0f });
		engine::render_command::clear();

		// Render the menu text
		const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
		m_text_manager->render_text(text_shader, "Game Over! You Died",
			engine::application::window().width() / 2.0f - 200.0f,
			engine::application::window().height() / 2.f,
			1.0f, glm::vec4(1.f, 0.f, 0.f, 1.f));
		//show score
		m_text_manager->render_text(text_shader, "Score: " + std::to_string(m_score),
			engine::application::window().width() / 2.0f - 200.0f,
			engine::application::window().height() / 2.0f - 50.0f,
			0.5f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		//show time paused
		int timer_seconds = static_cast<int>(m_timer);
		m_text_manager->render_text(text_shader, "Time Survived: " + std::to_string(timer_seconds) + " Seconds",
			engine::application::window().width() / 2.0f - 200.0f,
			engine::application::window().height() / 2.0f - 70.0f,
			0.5f, glm::vec4(1.f, 1.f, 1.f, 1.f));
		return; // Skip the rest of the rendering if the menu is active
	}

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("fog_on", m_fog);

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(mesh_shader, m_terrain);
	
	//point light
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gNumPointLights", (int)num_point_lights);

	m_pointLight.submit(mesh_shader, 0);
	m_pointLight2.submit(mesh_shader, 1);

	//spot light
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gNumSpotLights", (int)num_spot_lights);
	m_spotLight.submit(mesh_shader, 0);


	//Seed a random number generator with the current time, whenever it's picked up, random generate number, updating position. Create new pickup and update position of new pickup
	//Render the 3d pentagon object
	if (m_pentagon_pickup->active()) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);
		m_pentagon_pickup->textures().at(0)->bind();
		glm::mat4 pentagon_pickup_transform(1.0f);
		pentagon_pickup_transform = glm::translate(pentagon_pickup_transform, m_pentagon_pickup->position() + glm::vec3(0.f, 1.f, 0.f));
		pentagon_pickup_transform = glm::rotate(pentagon_pickup_transform, m_pentagon_pickup->rotation_amount(), m_pentagon_pickup->rotation_axis());
		//Point light
		engine::renderer::submit(mesh_shader, m_pentagon->meshes().at(0), pentagon_pickup_transform);
		m_pointLight.Position = m_pentagon_pickup->position() + glm::vec3(0.0f, 1.5f, 0.0f);
		m_pointLight.Color = glm::vec3(1.0f, 1.0f, 0.0f);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", false);
	}

	// Render the heart object pickup
	if (m_heart_pickup->active()) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);
		m_heart_pickup->textures().at(0)->bind();
		glm::mat4 heart_pickup_transform(1.0f);
		heart_pickup_transform = glm::translate(heart_pickup_transform, m_heart_pickup->position() + glm::vec3(0.f, 1.f, 0.f));
		heart_pickup_transform = glm::rotate(heart_pickup_transform, m_heart_pickup->rotation_amount(), m_heart_pickup->rotation_axis());
		engine::renderer::submit(mesh_shader, m_heart_pickup->meshes().at(0), heart_pickup_transform);
		m_pointLight2.Position = m_heart_pickup->position() + glm::vec3(0.0f, 1.5f, 0.0f);
		m_pointLight2.Color = glm::vec3(1.0f, 0.0f, 0.0f);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", false);
	}

	// Render the cone object pickup
	if (m_cone_pickup->active()) {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);
		m_cone_pickup->textures().at(0)->bind();
		glm::mat4 cone_transform(1.0f);
		cone_transform = glm::translate(cone_transform, m_cone_pickup->position() + glm::vec3(0.f, 0.5f, 0.f));
		cone_transform = glm::rotate(cone_transform, glm::radians(90.f), glm::vec3(-1.f, 0.f, 0.f));
		cone_transform = glm::rotate(cone_transform, m_cone_pickup->rotation_amount(), glm::vec3(0.f, 0.f, 1.f));
		cone_transform = glm::scale(cone_transform, m_cone_pickup->scale());
		engine::renderer::submit(mesh_shader, m_cone_pickup->meshes().at(0), cone_transform);
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", false);
	}

	// Render the cylinder object
	glm::mat4 cylinder_transform(1.0f);
	cylinder_transform = glm::translate(cylinder_transform, m_cylinder->position() + glm::vec3(0.f, 1.f, 0.f));
	cylinder_transform = glm::rotate(cylinder_transform, m_cylinder->rotation_amount(), m_cylinder->rotation_axis());
	cylinder_transform = glm::scale(cylinder_transform, m_cylinder->scale());
	engine::renderer::submit(mesh_shader, cylinder_transform, m_cylinder);

	//std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	m_lightsource_material->submit(mesh_shader);

	//spot light
	m_spotLight.Position = m_player.position() + glm::vec3(0.0f, 1.5f, 0.0f);
	m_spotLight.Direction = m_player.object()->forward(); //set to player forward vector;

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);

	m_projectile.on_render(mesh_shader);
	//m_grenade_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

	m_material->submit(mesh_shader);

	m_mannequin_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_player.object());
	//m_player_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

	// Render the zombie
	m_zombie_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_zombie);
	//m_zombie_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

	m_zombie2_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_zombie2);
	//m_zombie2_box.on_render(2.5f, 0.f, 0.f, mesh_shader);

	m_civilian_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_civilian);

    engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	int timer_seconds = static_cast<int>(m_timer);
	m_text_manager->render_text(text_shader, "Score: " + std::to_string(m_score), 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
	m_text_manager->render_text(text_shader, "Health: " + std::to_string(m_player.health()), 10.f, (float)engine::application::window().height() - 50.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
	m_text_manager->render_text(text_shader, "Grenades Left: " + std::to_string(m_grenades), 10.f, (float)engine::application::window().height() - 75.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
	m_text_manager->render_text(text_shader, "Time Survived: " + std::to_string(timer_seconds) + " Seconds", 10.f, (float)engine::application::window().height() - 100.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));

	engine::renderer::begin_scene(m_3d_camera, mesh_shader);
	m_billboard->on_render(m_3d_camera, mesh_shader);
	engine::renderer::end_scene();

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);
	m_cross_fade->on_render(mesh_shader);
	engine::renderer::end_scene();
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
		if (e.key_code() == engine::key_codes::KEY_G && m_grenades > 0)
		{
			m_projectile.toss(m_player, 0.5f);
			m_grenades--;
		}
		if (e.key_code() == engine::key_codes::KEY_1)
		{
			m_cross_fade->activate();
		}
		if (e.key_code() == engine::key_codes::KEY_2)
		{
			m_billboard->activate(glm::vec3(0.f, 5.f, -10.f), 4.f, 4.f);
		}
		if (e.key_code() == engine::key_codes::KEY_3)
		{
			m_fog = !m_fog;
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
	m_zombie2->set_position(glm::vec3(m_zombie2->position().x, m_heightmap->ground_height(m_zombie2->position()), m_zombie2->position().z));
	m_pentagon_pickup->set_position(glm::vec3(m_pentagon_pickup->position().x, m_heightmap->ground_height(m_pentagon_pickup->position()), m_pentagon_pickup->position().z));
	m_ball->set_position(glm::vec3(m_ball->position().x, m_heightmap->ground_height(m_ball->position()), m_ball->position().z));
	m_cylinder->set_position(glm::vec3(m_cylinder->position().x, m_heightmap->ground_height(m_cylinder->position()), m_cylinder->position().z));
	m_heart_pickup->set_position(glm::vec3(m_heart->position().x, m_heightmap->ground_height(m_heart->position()), m_heart->position().z));
	m_civilian->set_position(glm::vec3(m_civilian->position().x, m_heightmap->ground_height(m_civilian->position()), m_civilian->position().z));
	m_cone_pickup->set_position(glm::vec3(m_cone_pickup->position().x, m_heightmap->ground_height(m_cone_pickup->position()), m_cone_pickup->position().z));
}
