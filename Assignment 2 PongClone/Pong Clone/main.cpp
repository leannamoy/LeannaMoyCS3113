/**
* Author: Leanna Moy
* Assignment: Pong Clone
* Date due: 2023-10-21, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

#include <ctime>
#include "cmath"
#include <vector>

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.5f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";


const float MILLISECONDS_IN_SECOND = 1000.0;

const int NUMBER_OF_TEXTURES = 1;   // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;   // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;   // this value MUST be zero

const char PLAYER_SPRITE_FILEPATH[] = "assets/p1cat.png";
const char PLAYER2_FILEPATH[] = "assets/p2cat.png";
const char BALL_FILEPATH[] = "assets/ball.png";
const char P1WIN_FILEPATH[] = "assets/p1 wins.png";
const char P2WIN_FILEPATH[] = "assets/p2 wins.png";
const char START_TEXT_FILEPATH[] = "assets/start text.png";


SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
GLuint        g_player_texture_id, player2_texture_id, ball_texture_id, p1w_id, p2w_id, start_text_id;
glm::mat4     g_view_matrix,
g_model_matrix,
g_projection_matrix,
player1_middle, player2_middle, ball_model_matrix;

glm::mat4 player1_top, player1_bottom, player2_top, player2_bottom;
glm::mat4 p1_win_mm, p2_win_mm, start_text_mm;

float g_previous_ticks = 0.0f;

// ––––––––––––––––––––––––––––Player1 Stuff––––––––––––––––––––––––––––––– //
glm::vec3 player1_mid_pos = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 player1_top_pos = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 player1_bot_pos = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 player1_movement = glm::vec3(0.0f, 0.0f, 0.0f);     

float g_player_speed = 5.0f;  // move 1 unit per second        

// ––––––––––––––––––––––––––––Player2 Stuff––––––––––––––––––––––––––––––– //
glm::vec3 player2_mid_pos = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 player2_top_pos = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 player2_bot_pos = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 player2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cpu_movement = glm::vec3(0.0f, 1.0f, 0.0f);
bool cpu_mode = false;
glm::vec3 ytranslation = glm::vec3(0.0f, 0.3f, 0.0f); //consistent distance between 3 model matrices

// ––––––––––––––––––––––––––––Ball/Game Stuff––––––––––––––––––––––––––––––– //
glm::vec3 ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
const float MINIMUM_COLLISION_DISTANCE = 0.5f;
bool game_started = false;
bool game_won = false;
const float ROT_ANGLE = 0.2f;
int winner = 0; // an int for who won
const double borderx = 5.0;
const double bordery = 3.5;

// ––––––––––––––––––––––––––––Check Collision––––––––––––––––––––––––––––––– //
bool check_collision(glm::vec3& position_a, glm::vec3& position_b, bool middle) {
    //bool middle: making the middle part of the paddle easier to hit than the other 2 parts
    
    const float distance = (middle) ? MINIMUM_COLLISION_DISTANCE : MINIMUM_COLLISION_DISTANCE-0.01;
    

    return sqrt(
        pow(position_b[0] - position_a[0], 2) +
        pow(position_b[1] - position_a[1], 2)
    ) < distance;
}

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}



void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Pongy",
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

    g_player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
    player2_texture_id = load_texture(PLAYER2_FILEPATH);
    ball_texture_id = load_texture(BALL_FILEPATH);

    p1w_id = load_texture(P1WIN_FILEPATH);
    p2w_id = load_texture(P2WIN_FILEPATH);
    start_text_id = load_texture(START_TEXT_FILEPATH);

    g_model_matrix = glm::mat4(1.0f); ///???????????????
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    player1_top = glm::mat4(1.0f);
    player1_middle = glm::mat4(1.0f);
    player1_bottom = glm::mat4(1.0f);

    player2_top = glm::mat4(1.0f);
    player2_middle = glm::mat4(1.0f);
    player2_bottom = glm::mat4(1.0f);


    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    player1_movement = glm::vec3(0.0f);      
    player2_movement = glm::vec3(0.0f);
    
// –––––––––––––––––––––––––––––––– KEYSTROKES ––––––––––––––––––––––––– //
                                                                         
    SDL_Event event;                                                         
    while (SDL_PollEvent(&event))                                            
    {                                                                        
        switch (event.type)                                                  
        {                                                                    
            // End game                                                      
        case SDL_QUIT:                                                   
        case SDL_WINDOWEVENT_CLOSE:                                      
            g_game_is_running = false;                                   
            break;                                                       
            //
        case SDL_KEYDOWN:                                                
            switch (event.key.keysym.sym)                                
            {                                                                                              
                                                      
                
            case SDLK_q:                                             
                // Quit the game with a keystroke                    
                g_game_is_running = false;                           
                break;                                               
                //
            case SDLK_t:
                //turn player2 into cpu
                cpu_mode = !cpu_mode;

            default:                                                 
                break;                                               
            }                                                            
                                                                         
        default:                                                         
            break;                                                       
        }                                                                    
    }                                                                        
                                                                             
    // ––––––––––––––––––––––––––––––– KEY HOLD (P1) –––––––––––––––––––––––––––– //
                                                                             
    const Uint8* key_state = SDL_GetKeyboardState(NULL);                     
    //                                                                   
                                                                             
    if (key_state[SDL_SCANCODE_W])                                          
    {                                                                        
        player1_movement.y = 1.0f;                                          
    }                                                                        
    else if (key_state[SDL_SCANCODE_S])                                   
    {                                                                        
        player1_movement.y = -1.0f;                                         
    }                                                                        

    //––––––––––––––––––––––––––––––– P2 ––––––––––––––––––––––––––––//
    if (key_state[SDL_SCANCODE_UP])
    {
        player2_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        player2_movement.y = -1.0f;
    }

    //––––––––––––––––––––––––––––––– OTHER ––––––––––––––––––––––––––––//
    if (key_state[SDL_SCANCODE_SPACE]) { //starts the game
        ball_movement.x = -1.0f;
        ball_movement.y = 0.0f;
        game_won = false;
        game_started = true;
    }

}

void move3parts(glm::mat4& top, glm::mat4& mid, glm::mat4& bot, glm::vec3& position) {
    mid = glm::translate(mid, position);
    top = glm::translate(top, position + ytranslation);
    bot = glm::translate(bot, position - ytranslation);
}

void updatePositionVecs(glm::vec3& top, glm::vec3& mid, glm::vec3& bot) { //update position vectors based on mid
    top = mid + ytranslation;
    bot = mid - ytranslation;
}

void toggleText(glm::mat4& text_mm, bool show) {
    if (show) {
        text_mm = glm::scale(text_mm, glm::vec3(4.0f, 4.0f, 0.0f));
    }
    else {
        text_mm = glm::scale(text_mm, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void resetBall() {
    ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
    ball_movement.x = 0.0f;
    ball_movement.y = 0.0f;
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    glm::vec3 scale_vector = glm::vec3(1.3f, 1.3f, 1.3f);
    glm::vec3 ball_scale_vector = glm::vec3(0.3f, 0.3f, 0.3f);

    start_text_mm = glm::mat4(1.0f);
    p1_win_mm = glm::mat4(1.0f);
    p2_win_mm = glm::mat4(1.0f);

    if (winner == 0) {
        toggleText(p1_win_mm, false);
        toggleText(p2_win_mm, false);
    }
    if (game_started) {
        toggleText(start_text_mm, false);
        winner = 0;
    }
    else
    {
        toggleText(start_text_mm, true);
        if (winner == 1) toggleText(p1_win_mm, true);
        else if (winner == 2) toggleText(p2_win_mm, true);
    }

    player1_mid_pos += player1_movement * g_player_speed * delta_time;   //
    if (player1_mid_pos.y > 3.0) player1_mid_pos.y = 3.0;
    if (player1_mid_pos.y < -3.0) player1_mid_pos.y = -3.0;

    player1_middle = glm::mat4(1.0f);                         
    player1_top = glm::mat4(1.0f);
    player1_bottom = glm::mat4(1.0f);

    move3parts(player1_top, player1_middle, player1_bottom, player1_mid_pos);
    updatePositionVecs(player1_top_pos, player1_mid_pos, player1_bot_pos);
    //
    // –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––– //


    //PLAYER 2 MOVEMENTS (checking for cpu mode)//
    if (!cpu_mode) {

        player2_mid_pos += player2_movement * g_player_speed * delta_time;
        if (player2_mid_pos.y > 3.0)  player2_mid_pos.y = 3.0; 
        if (player2_mid_pos.y < -3.0) player2_mid_pos.y = -3.0; 
    }
    else {
        if (player2_mid_pos.y > 3.0) {
            player2_mid_pos.y = 3.0;
            cpu_movement.y = -1.0;
        }
        if (player2_mid_pos.y < -3.0) {
            player2_mid_pos.y = -3.0;
            cpu_movement.y = 1.0;
        }
        player2_mid_pos += cpu_movement * g_player_speed * delta_time;
    }

    player2_middle = glm::mat4(1.0f);
    player2_top = glm::mat4(1.0f);
    player2_bottom = glm::mat4(1.0f);

    move3parts(player2_top, player2_middle, player2_bottom, player2_mid_pos);
    updatePositionVecs(player2_top_pos, player2_mid_pos, player2_bot_pos);

    //--------------------------------------------BALL COLLISIONS--------------------------------------
  
    //Middle paddle collision
    if (check_collision(ball_position, player1_mid_pos, true) || check_collision(ball_position, player2_mid_pos, true)) { 
        ball_movement.x = (ball_movement.x <= 0) ? 1.4f : -1.4f;
        if (ball_movement.y >= 0) ball_movement.y = -0.05f;
        else if (ball_movement.y < 0) ball_movement.y = 0.05f;
    }

    //Top paddle collision
    else if (check_collision(ball_position, player1_top_pos, false) || check_collision(ball_position, player2_top_pos, false)) {
        ball_movement.x = -ball_movement.x;
        ball_movement.y = 1.0f;
    }

    //Bottom paddle collision
    else if (check_collision(ball_position, player1_bot_pos, false) || check_collision(ball_position, player2_bot_pos, false)) {
        ball_movement.x = -ball_movement.x;
        ball_movement.y = -1.0f;
    }

    //Checking vertical border hits
    if (ball_position.y > bordery) {
        ball_position.y = 3.5;
        ball_movement.y = -ball_movement.y;
    }

    else if (ball_position.y < -bordery) {
        ball_position.y = -3.5;
        ball_movement.y = -ball_movement.y;
    }

    if (ball_position.x < -borderx){ //P2 wins
        winner = 2;
        game_started = false;
        game_won = true;
        resetBall();
    }
        
    else if (ball_position.x > borderx) { //P1 wins
        winner = 1;
        game_started = false;
        game_won = true;
        resetBall();
    }

    ball_position += ball_movement * g_player_speed * delta_time;

    ball_model_matrix = glm::mat4(1.0f);
    ball_model_matrix = glm::translate(ball_model_matrix, ball_position);
    ball_model_matrix = glm::rotate(ball_model_matrix, ROT_ANGLE, ball_position);


    //scalings
    player1_middle = glm::scale(player1_middle, scale_vector);
    player2_middle = glm::scale(player2_middle, scale_vector);
    ball_model_matrix = glm::scale(ball_model_matrix, ball_scale_vector);
    } //end update
    


void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(player1_middle, g_player_texture_id);
    draw_object(player2_middle, player2_texture_id);
    draw_object(ball_model_matrix, ball_texture_id);

    draw_object(start_text_mm, start_text_id);
    draw_object(p1_win_mm, p1w_id);
    draw_object(p2_win_mm, p2w_id);
    

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


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