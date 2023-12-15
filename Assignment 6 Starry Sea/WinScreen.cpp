/**
* Author: Leanna Moy
* Assignment: Starry Sea
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "WinScreen.h"
#include "Utility.h"

#define WIN_WIDTH 1
#define WIN_HEIGHT 1

GLuint win_text_texture_id;

unsigned  int WIN_DATA[] =
{
	0
};

WinScreen::~WinScreen()
{
	delete m_state.player;
	delete m_state.hook;
	delete m_state.map;
}

void WinScreen::initialise() {

	m_state.next_scene_id = -1;
	//red, blue, green, opacity
	glClearColor(0.0, 0.0, 0.0, 1.00);

	win_text_texture_id = Utility::load_texture("assets/fonts/font1.png");
	
	m_state.player = new Entity();
	m_state.player->set_entity_type(PLAYER);
	m_state.player->set_position(glm::vec3(2.0f, 0.0f, 0.0f));
	m_state.hook = new Entity();
	m_state.hook->set_entity_type(HOOK);
	m_state.player->set_hook(m_state.hook);
	m_state.player->set_width(2.0f);
	m_state.player->set_height(2.0f);

	m_state.player->set_scale(9.0f);
	
	m_state.player->m_texture_id = Utility::load_texture("assets/images/win_screen.png");
	GLuint map_texture_id = Utility::load_texture("assets/images/A_tileset.png");
	m_state.map = new Map(WIN_WIDTH, WIN_HEIGHT, WIN_DATA, map_texture_id, 1.0f, 4, 1);

	//bgm
	Mix_CloseAudio();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	m_state.bgm = Mix_LoadMUS("assets/audio/the-road-ahead-zoo-main-version-3629-02-14.mp3");
	Mix_PlayMusic(m_state.bgm, -1);
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

}

void WinScreen::update(float delta_time) {
	m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map);
}

void WinScreen::render(ShaderProgram* program) {
	m_state.player->render(program);
	Utility::draw_text(program, win_text_texture_id, std::string("You did it!"), 0.35f, 0.0f, glm::vec3(0.375f, 0.0f, 0.0f));
}