#pragma once
#include "Scene.h"

class LevelA : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 3;
    int level_num = 1;

    glm::vec3 start_pos = glm::vec3(2.0f, -5.0f, 0.0f);

    // ————— CONSTRUCTOR ————— //
    ~LevelA();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time, int& lives) override;
    void render(ShaderProgram* program) override;
};