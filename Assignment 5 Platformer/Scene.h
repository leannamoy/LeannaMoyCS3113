#pragma once
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map* map;
    Entity* player;
    Entity* enemies;
    Entity* others;  //other map interactables, such as springs
    Entity* goal;

    // ————— AUDIO ————— //
    Mix_Music* bgm;
    Mix_Chunk* jump_sfx;
    Mix_Chunk* game_won_sfx;
    Mix_Chunk* game_lost_sfx;

    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;

    // ------ other ----- //
    int ENEMY_COUNT = 4;
};

class Scene {
public:
    // ————— ATTRIBUTES ————— //
    int ENEMY_COUNT = 4;
    int level_num;
    glm::vec3 start_pos;

    bool goal_touched = false;
    GameState m_state;

    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time, int& lives) = 0;
    virtual void render(ShaderProgram* program) = 0;

    // ————— GETTERS ————— //
    GameState const get_state()             const { return m_state; }
    int       const get_number_of_enemies() const { return ENEMY_COUNT; }
    bool      const get_goal_touched()      const { return goal_touched; }
};