#pragma once
#include "Scene.h"
#include "Utility.h"

class LevelA : public Scene {
public:
	// STATIC ATTRIBUTES
	int next_scene_id = -1;
	int FISH_COUNT = 5;
	int ROCK_COUNT  = 1;
	int BLACKHOLE_COUNT =1;
	int UFO_COUNT = 1;
	bool canPlayHookAudioAgain = true;

	glm::vec3 start_pos = glm::vec3(0.0f, 0.0f, 0.0f); //should be 0 for whatever reason; useless deslete later?
	glm::vec3 goal_pos = glm::vec3(15+5.0f, -5.0f, 0.0f);

	GLuint text_texture_id = Utility::load_texture("assets/fonts/font1.png");

	// Constructor
	~LevelA();

	// Methods
	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;

};