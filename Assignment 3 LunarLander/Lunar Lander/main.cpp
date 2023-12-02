/**
* Author: Leanna Moy
* Assignment: Lunar Lander
* Date due: 2023-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define NUMBER_OF_ENEMIES 3
#define FIXED_TIMESTEP 0.0166666f
#define ACC_OF_GRAVITY -9.81f
#define PLATFORM_COUNT 20
#define GOAL_COUNT 5
#define TOTALPLATFORMS (GOAL_COUNT+PLATFORM_COUNT)

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include <string>
#include <cstdlib>

// ————— STRUCTS AND ENUMS —————//
struct GameState
{
    Entity* player;
    Entity* platforms;
};

// ————— CONSTANTS ————— //
const int WINDOW_WIDTH = 1280,
WINDOW_HEIGHT = 960;

const float BG_RED = 0.15f,
BG_BLUE = 0.12f,
BG_GREEN = 0.15f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const char  SPRITESHEET_FILEPATH[] = "assets/george_0.png",
PLATFORM_FILEPATH[] = "assets/platform.png",
GOAL_FILEPATH[] = "assets/goal.png",
FONT_FILEPATH[] = "assets/font1.png";

const int FONTBANK_SIZE = 16;

const int NUMBER_OF_TEXTURES = 1;  // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;  // this value MUST be zero

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_time_accumulator = 0.0f;
//------MY NEW STUFF------//
float fuel = 100;
GLuint g_text_texture_id;
GLuint goal_texture_id;

// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Lunar Lander",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ————— PLAYER ————— //
    // Existing
    g_game_state.player = new Entity();
    g_game_state.player->set_position(glm::vec3(-9.0f, 6.0f, 0.0f));
    g_game_state.player->set_movement(glm::vec3(0.0f));
    g_game_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.1, 0.0f));
    g_game_state.player->set_speed(1.0f);
    g_game_state.player->m_texture_id = load_texture(SPRITESHEET_FILEPATH);
    g_game_state.player->set_entity_type(PLAYER);

    // Walking
    g_game_state.player->m_walking[g_game_state.player->LEFT] = new int[4] { 1, 5, 9, 13 };
    g_game_state.player->m_walking[g_game_state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };

    g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];  // start George looking right
    g_game_state.player->m_animation_frames = 4;
    g_game_state.player->m_animation_index = 0;
    g_game_state.player->m_animation_time = 0.0f;
    g_game_state.player->m_animation_cols = 4;
    g_game_state.player->m_animation_rows = 4;
    g_game_state.player->set_height(0.9f);
    g_game_state.player->set_width(0.9f);

    // Jumping
    g_game_state.player->m_boosting_power = 0.25f;

    // ————— PLATFORM ————— //
    g_game_state.platforms = new Entity[TOTALPLATFORMS];
    srand(time(0)); //for randomizing
    int heights = 1; //1 different heights?

    for (int i = 0; i < TOTALPLATFORMS; i++)
    {
        g_game_state.platforms[i].m_texture_id = load_texture(PLATFORM_FILEPATH);
        g_game_state.platforms[i].set_position(glm::vec3(i - 9.5f, float((rand() % heights))-3.0f, 0.0f));
        g_game_state.platforms[i].update(0.0f, NULL, 0);
        g_game_state.platforms[i].set_entity_type(PLATFORM);
    }
    
    // ----- GOAL PLATFORMS ----- //
    int total = TOTALPLATFORMS;
    
    for (int i = 0; i < GOAL_COUNT; i++){
        int random = rand() % total;
        g_game_state.platforms[random].m_texture_id = load_texture(GOAL_FILEPATH);
        g_game_state.platforms[random].set_entity_type(GOAL);
    }
    
    // ————— GENERAL ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_text_texture_id = load_texture(FONT_FILEPATH);
}

void process_input()
{
    g_game_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
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
                if (fuel) {
                    g_game_state.player->m_is_boosting = true;
                    fuel -= 0.5;
                }
            case SDLK_LEFT:
                if (fuel) {
                    fuel -= 0.5;
                }
            case SDLK_RIGHT:
                if (fuel) {
                    fuel -= 0.5;
                }
            default:
                break;
            }


        default:
            break;
        }
    }
    

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        if (fuel) {
            g_game_state.player->move_left();
            g_game_state.player->m_moving_left = true;
            g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->LEFT];
        }
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        if (fuel) {
            g_game_state.player->move_right();
            g_game_state.player->m_moving_right = true;
            g_game_state.player->m_animation_indices = g_game_state.player->m_walking[g_game_state.player->RIGHT];
        }
    }

    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_game_state.player->get_movement()) > 1.0f)
    {
        g_game_state.player->set_movement(glm::normalize(g_game_state.player->get_movement()));
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    delta_time += g_time_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.platforms, TOTALPLATFORMS);
        delta_time -= FIXED_TIMESTEP;
    }


    g_time_accumulator = delta_time;
}

void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;
        // For every character...
        for (int i = 0; i < text.size(); i++) {
            // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
            //    relative to the whole sentence)
            int spritesheet_index = (int)text[i];  // ascii value of character
            float offset = (screen_size + spacing) * i;

            // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
            float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
            float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

                // 3. Inset the current pair in both vectors
                vertices.insert(vertices.end(), {
                    offset + (-0.5f * screen_size), 0.5f * screen_size,
                    offset + (-0.5f * screen_size), -0.5f * screen_size,
                    offset + (0.5f * screen_size), 0.5f * screen_size,
                    offset + (0.5f * screen_size), -0.5f * screen_size,
                    offset + (0.5f * screen_size), 0.5f * screen_size,
                    offset + (-0.5f * screen_size), -0.5f * screen_size,
                    });

                texture_coordinates.insert(texture_coordinates.end(), {
                    u_coordinate, v_coordinate,
                    u_coordinate, v_coordinate + height,
                    u_coordinate + width, v_coordinate,
                    u_coordinate + width, v_coordinate + height,
                    u_coordinate + width, v_coordinate,
                    u_coordinate, v_coordinate + height,
                    });
        }

        // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void render()
{
    // ————— GENERAL ————— //
    glClear(GL_COLOR_BUFFER_BIT);

    // ————— PLAYER ————— //
    g_game_state.player->render(&g_shader_program);

    // ————— PLATFORM ————— //
    for (int i = 0; i < PLATFORM_COUNT; i++) g_game_state.platforms[i].render(&g_shader_program);

    // ————— GENERAL ————— //
    draw_text(&g_shader_program, g_text_texture_id, std::string("FUEL: "), 0.25f, 0.0f, glm::vec3(-1.25f, 2.0f, 0.0f));
    draw_text(&g_shader_program, g_text_texture_id, std::to_string(static_cast<int>(fuel)) , 0.25f, 0.0f, glm::vec3(0.0f, 2.0f, 0.0f));
    
    if (g_game_state.player->get_touched_goal() == true) {
        draw_text(&g_shader_program, g_text_texture_id, std::string("mission accomplished!"), 0.25f, 0.0f, glm::vec3(-2.0f, 1.5f, 0.0f));
    }
    else if (g_game_state.player->get_touched_platform() == true) {
        draw_text(&g_shader_program, g_text_texture_id, std::string("mission failed!"), 0.25f, 0.0f, glm::vec3(-2.0f, 1.5f, 0.0f));
    }
    SDL_GL_SwapWindow(g_display_window);

    
}

void shutdown() { SDL_Quit(); }

// ————— DRIVER GAME LOOP ————— /
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}