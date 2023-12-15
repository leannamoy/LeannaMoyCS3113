#pragma once
#include "Scene.h"

class LevelB : public Scene {
public:
	// STATIC ATTRIBUTES
	int next_scene_id = -1;
	int FISH_COUNT = 11;
	int ROCK_COUNT = 3;
	int BLACKHOLE_COUNT = 4;
	int UFO_COUNT = 3;
	bool canPlayHookAudioAgain = true;

	glm::vec3 start_pos = glm::vec3(0.0f, 0.0f, 0.0f); //should be 0 for whatever reason; useless deslete later?
	glm::vec3 goal_pos = glm::vec3(45.0f, -5.0f, 0.0f);

	// Constructor
	~LevelB();

	// Methods
	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;

};