#pragma once
#include "Scene.h"

class LevelB : public Scene {
public:
	// Static Attributes  //
	int ENEMY_COUNT = 4;
	int OTHER_COUNT = 2;
	int level_num = 2;

	glm::vec3 start_pos = glm::vec3(2.0f, 0.0f, 0.0f);
	// Deconstructor //
	~LevelB();

	// Methods //
	void initialise() override;
	void update(float delta_time, int& lives) override;
	void render(ShaderProgram* program) override;
};