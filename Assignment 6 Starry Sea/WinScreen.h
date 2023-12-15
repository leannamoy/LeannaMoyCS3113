#pragma once
#include "Scene.h"

class WinScreen : public Scene {
public:

	~WinScreen();

	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram* program) override;
};