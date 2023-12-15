#pragma once
#include "Scene.h"
#include "Utility.h"

class LevelC : public Scene {
public:
	// STATIC ATTRIBUTES
	int next_scene_id = 0;
	int FISH_COUNT = 12;
	int ROCK_COUNT = 6;
	int BLACKHOLE_COUNT = 6;
	int UFO_COUNT = 4;
	bool canPlayHookAudioAgain = true;

	glm::vec3 start_pos = glm::vec3(0.0f, 0.0f, 0.0f); //should be 0 for whatever reason; useless deslete later?
	glm::vec3 goal_pos = glm::vec3(53.0f, -5.0f, 0.0f);

	GLuint text_texture_id = Utility::load_texture("assets/fonts/font1.png");

	// Constructor
	~LevelC();

	// Methods
	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;

};