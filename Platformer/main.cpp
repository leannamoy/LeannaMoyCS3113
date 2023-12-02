/**
* Author: Leanna Moy
* Assignment: Rise of the AI
* Date due: 2023-11-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Menu.h"

// ————— CONSTANTS ————— //
const int   WINDOW_WIDTH = 640*2,
WINDOW_HEIGHT = 480*2;

float BG_RED = 0.345f,
BG_BLUE = 0.259f,
BG_GREEN = 0.534f,
BG_OPACITY = 1.0f;

const int   VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
Menu* g_menu;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
int lives = 2; // one less
Scene* g_levels[4];

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

//for view matrix
float x;
float y;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

GLuint g_text_texture_id;

//to ensure these audios only play once
bool lose_audio_played = false;
bool win_audio_played = false;
bool game_started = false;

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    g_current_scene->initialise();
}

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Underwater Treasure!!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.m_program_id);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_text_texture_id = Utility::load_texture("assets/fonts/font1.png");

    // ————— LEVELS SETUP ————— //
    g_menu = new Menu();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
   
    g_levels[0] = g_menu;
    g_levels[1] = g_level_a; 
    g_levels[2] = g_level_b;
    g_levels[3] = g_level_c;
    switch_to_scene(g_menu); //start at menu


    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
}

void process_input()
{
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                // ————— JUMPING ————— //
                if (g_current_scene->m_state.player->m_collided_bottom)
                {
                    g_current_scene->m_state.player->m_is_jumping = true;
                    Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
                }
                break;

            case SDLK_RETURN:
                if (!game_started) g_current_scene->m_state.next_scene_id = 1;
                game_started = true;
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    // ————— KEY HOLD ————— //
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_state.player->move_left();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_state.player->move_right();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }

    // ————— NORMALISATION ————— //
    if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
    }
}

bool checkGameWin();

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP, lives);

        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;


    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
       // g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
        x = -g_current_scene->m_state.player->get_position().x;
    }
    
    else {
       // g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        x = -5;
    }

    if (g_current_scene->m_state.player->get_position().y < -6.8f) {
        //g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, -g_current_scene->m_state.player->get_position().y, 0));
        y = -g_current_scene->m_state.player->get_position().y;
    }
    else {
        y = 3.75;
    }
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(x, y, 0));


    if (checkGameWin() == true) {
        if (!win_audio_played) Mix_PlayChannel(-1, g_current_scene->m_state.game_won_sfx, 0);
        win_audio_played = true;
    }
        
    else if (g_current_scene->m_state.player->get_active() == false && lives == 0) {
        if (!lose_audio_played) Mix_PlayChannel(-1, g_current_scene->m_state.game_lost_sfx, 0);
        lose_audio_played = true;
    }
    
}

bool checkGameWin() {
        //this will happen in the third level.
    if (g_current_scene->get_goal_touched()) return true;
    return false;
}


void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    if (checkGameWin() == true) {
        Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("You Win!"), 0.5f, 0.0f, g_current_scene->m_state.player->get_position() + glm::vec3(-1.5f, 2.0f, 0.0f));
    }
    else if (g_current_scene->m_state.player->get_active() == false && lives == 0) {
        Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("You Lose!"), 0.5f, 0.0f, g_current_scene->m_state.player->get_position() + glm::vec3(-1.5f, 2.0f, 0.0f));
    }
    else {
        Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("Lives: " + std::to_string(lives + 1)), 0.5f, 0.0f, glm::vec3(g_current_scene->m_state.player->get_position().x-1.5, g_current_scene->m_state.player->get_position().y+1, 0));
        
        //for testing
        //float x_coord = g_current_scene->m_state.player->get_position().x;
        //float y_coord = g_current_scene->m_state.player->get_position().y;
        //Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("(" + std::to_string(x_coord) + ", " + std::to_string(y_coord) + ")"), 0.25f, 0.0f, glm::vec3(g_current_scene->m_state.player->get_position().x - 1.5, g_current_scene->m_state.player->get_position().y + 1, 0));
    }
    
   
    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_menu;
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();

        if (g_current_scene->m_state.next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id]);

        render();
    }

    shutdown();
    return 0;
}