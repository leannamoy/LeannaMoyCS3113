#pragma once
#include "Scene.h"

class LevelC : public Scene {
public:
	//Static Attributes
	int ENEMY_COUNT = 5;
	int level_num = 3;
	glm::vec3 start_pos = glm::vec3(2.0f, -2.0f, 0.0f);

	//Destructor
	~LevelC();
	
	//Methods
	void initialise() override;
	void update(float delta_time, int& lives) override;
	void render(ShaderProgram* program) override;
};