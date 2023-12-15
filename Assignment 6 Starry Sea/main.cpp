/**
* Author: Leanna Moy
* Assignment: Starry Sea
* Date due: 2023-12-15, 11:59pm
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
#include "WinScreen.h"

// ————— CONSTANTS ————— //
const int   WINDOW_WIDTH = 640 * 2,
WINDOW_HEIGHT = 480 * 2;

float BG_RED = 0.0f,
BG_BLUE = 0.0f,
BG_GREEN = 0.0f,
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
LevelA* g_level_A;
LevelB* g_level_B;
LevelC* g_level_C;
WinScreen* g_winScreen;

Scene* g_levels[5];

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

//for view matrix + scrolling?
float x;
float y;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

GLuint g_text_texture_id; // font

//to ensure these audios only play once
bool lose_audio_played = false;
bool win_audio_played = false;
bool game_started = false;
bool canPlayHookAudioAgain = true;

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
    g_display_window = SDL_CreateWindow("Starry Sea",
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
    g_level_A = new LevelA();
    g_level_B = new LevelB();
    g_level_C = new LevelC();
    g_winScreen = new WinScreen();

    g_levels[0] = g_menu;
    g_levels[1] = g_level_A;
    g_levels[2] = g_level_B;
    g_levels[3] = g_level_C;
    g_levels[4] = g_winScreen;
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
                // ————— FISHING ROD ————— //
                // space will launch the hook in the direction it's facing.
                //not yet launched/hooked
                if (!g_current_scene->m_state.hook->get_isLaunched()
                       && g_current_scene->m_state.hook->get_position()==g_current_scene->m_state.hook->get_og_pos()) {
                    g_current_scene->m_state.hook->set_extended(true);
                }
                else if (g_current_scene->m_state.hook->get_isHooked()) {
                    //but if hooked, then pull back!
                    g_current_scene->m_state.hook->set_reeling(true);

                }

                break;

            case SDLK_RETURN:
                if (!game_started) g_current_scene->m_state.next_scene_id = 1;
                game_started = true;
                break;

            case SDLK_r:
                //reset hook position for debugging purposes!
                g_current_scene->m_state.hook->set_position(
                    glm::vec3(g_current_scene->m_state.player->get_position().x + 1.0f,
                        g_current_scene->m_state.player->get_position().y, 0.0f));
                g_current_scene->m_state.hook->set_isLaunched(false);
                g_current_scene->m_state.hook->set_extended(false);
                g_current_scene->m_state.hook->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                break;

            case SDLK_u:
                //change to abduct for debugging purposes!
                g_current_scene->m_state.ufos[0].set_ai_state(ABDUCT);


            default:
                break;
            }

        default:
            break;
        }
    }

    // ————— KEY HOLD ————— //
    //holding left and right will rotate the hook
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        if (glm::length(g_current_scene->m_state.hook->get_movement()) == 0.0f
            && g_current_scene->m_state.hook->get_isLaunched()==false)
                 g_current_scene->m_state.hook->rotate_left();
      //  g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
       if (glm::length(g_current_scene->m_state.hook->get_movement()) == 0.0f
           && g_current_scene->m_state.hook->get_isLaunched() == false)
                  g_current_scene->m_state.hook->rotate_right();
      //  g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }
   
    //FOR DEVELOPING PURPOSES. 
    /*
    if (key_state[SDL_SCANCODE_W]) {
        g_current_scene->m_state.player->move_up();
    }
    if (key_state[SDL_SCANCODE_S]) {
        g_current_scene->m_state.player->move_down();
    }
    if (key_state[SDL_SCANCODE_D]) {
        g_current_scene->m_state.player->move_right();
    }
    if (key_state[SDL_SCANCODE_A]) {
        g_current_scene->m_state.player->move_left();
    }*/

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
        g_current_scene->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, -g_current_scene->m_state.player->get_position());

    //sfx?

    if (g_current_scene->m_state.hook->get_isHooked() == false) {
        canPlayHookAudioAgain = true;
    }

    else if (g_current_scene->m_state.hook->get_isHooked() && canPlayHookAudioAgain) {
        Mix_PlayChannel(-1, g_current_scene->m_state.hook_sfx, 0);
        canPlayHookAudioAgain = false;
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
       // Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("You Win!"), 0.5f, 0.0f, g_current_scene->m_state.player->get_position() + glm::vec3(-1.5f, 2.0f, 0.0f));
    }
    else if (g_current_scene->m_state.player->get_active() == false) {
        Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("You Lose!"), 0.5f, 0.0f, g_current_scene->m_state.player->get_position() + glm::vec3(-1.5f, 2.0f, 0.0f));
    }
    else {
        //for testing
        float x_coord = g_current_scene->m_state.player->get_position().x;
        float y_coord = g_current_scene->m_state.player->get_position().y;
    //    Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("(" + std::to_string(x_coord) + ", " + std::to_string(y_coord) + ")"), 0.25f, 0.0f, glm::vec3(g_current_scene->m_state.player->get_position().x - 1.5, g_current_scene->m_state.player->get_position().y + 1, 0));
    
       /* float x_coord2 = g_current_scene->m_state.player->get_movement().x;
        float y_coord2 = g_current_scene->m_state.player->get_movement().y;
        Utility::draw_text(&g_shader_program, g_text_texture_id, std::string("(" + std::to_string(x_coord2) + ", " + std::to_string(y_coord2) + ")"), 0.25f, 0.0f, glm::vec3(g_current_scene->m_state.hook->get_position().x - 1.5, g_current_scene->m_state.hook->get_position().y + 1.3, 0));
        Utility::draw_text(&g_shader_program, g_text_texture_id, std::string(std::to_string(g_current_scene->m_state.hook->get_rot_angle())), 0.25f, 0.0f, glm::vec3(g_current_scene->m_state.hook->get_position().x - 1.5, g_current_scene->m_state.hook->get_position().y + 1, 0));
    */
    }


    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    //delete g_menu;
    delete g_level_A;
    delete g_level_B;
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