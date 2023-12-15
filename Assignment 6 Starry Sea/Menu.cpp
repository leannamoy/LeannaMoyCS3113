/**
* Author: Leanna Moy
* Assignment: Starry Sea
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 1
#define LEVEL_HEIGHT 1

GLuint text_texture_id;

unsigned  int LEVEL_DATA[] =
{
	0
};

Menu::~Menu()
{
	delete m_state.player;
	delete m_state.hook;
	delete m_state.map;
}

void Menu::initialise() {

	//red, blue, green, opacity
	glClearColor(0.420, 0.220, 0.720, 1.00);

	text_texture_id = Utility::load_texture("assets/fonts/font1.png");
	m_state.next_scene_id = -1;
	m_state.player = new Entity();
	m_state.player->set_entity_type(PLAYER);
	m_state.player->set_position(glm::vec3(2.0f, 0.0f, 0.0f));
	m_state.hook = new Entity();
	m_state.hook->set_entity_type(HOOK);
	m_state.player->set_hook(m_state.hook);
	m_state.player->m_texture_id = Utility::load_texture("assets/images/astrocat.png");
	GLuint map_texture_id = Utility::load_texture("assets/images/A_tileset.png");
	m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 4, 1);
}

void Menu::update(float delta_time) {
	//m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map);
}

void Menu::render(ShaderProgram* program) {
	Utility::draw_text(program, text_texture_id, std::string("Starry Sea"), 0.55f, 0.0f, glm::vec3(-2.0f, 1.0f, 0.0f));
	Utility::draw_text(program, text_texture_id, std::string("Get back to your ship!"), 0.35f, 0.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
	Utility::draw_text(program, text_texture_id, std::string("Enter to start."), 0.35f, 0.0f, glm::vec3(-0.375f, -3.0f, 0.0f));
}