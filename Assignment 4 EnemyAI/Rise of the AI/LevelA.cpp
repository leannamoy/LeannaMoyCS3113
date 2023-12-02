/**
* Author: Leanna Moy
* Assignment: Rise of the AI
* Date due: 2023-11-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 8


unsigned int LEVEL_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
};

LevelA::~LevelA()
{
    delete[] m_state.enemies;
    delete[] m_state.projectiles;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeChunk(m_state.game_won_sfx);
    Mix_FreeChunk(m_state.game_lost_sfx);
    Mix_FreeMusic(m_state.bgm);
}


void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/images/tileset.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 4, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(2.0f, -5.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/images/player_sprite.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 0, 2, 4, 8 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 1, 3, 5, 7 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 2;
    m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    m_state.player->m_jumping_power = 5.0f;

    /**
     Enemies' stuff */
    GLuint skater_texture_id = Utility::load_texture("assets/images/skateboarder.png");
    GLuint shooter_texture_id = Utility::load_texture("assets/images/slingshot.png");
    GLuint jumper_texture_id = Utility::load_texture("assets/images/jumper.png");
    GLuint projectile_texture_id = Utility::load_texture("assets/images/rock.png");
    
    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(SKATER);
    m_state.enemies[0].set_ai_state(SKATING);
    m_state.enemies[0].m_texture_id = skater_texture_id;
    m_state.enemies[0].set_position(glm::vec3(9.0f, -5.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(1.0f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(SHOOTER);
    m_state.enemies[1].m_texture_id = shooter_texture_id;
    m_state.enemies[1].set_position(glm::vec3(8.0f, -1.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_speed(1.0f);
    m_state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    //m_state.enemies[2].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    //create a projectile
    m_state.enemies[2].set_entity_type(PROJECTILE);
    m_state.enemies[2].set_ai_state(SHOOTING);
    m_state.enemies[2].m_texture_id = projectile_texture_id;
    m_state.enemies[2].set_position(glm::vec3(8.0f, -6.0f, 0.0f));
    m_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_state.enemies[2].set_owner(&m_state.enemies[1]);
    m_state.enemies[2].set_speed(1.25f);
    m_state.enemies[2].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_state.enemies[3].set_entity_type(ENEMY);
    m_state.enemies[3].set_ai_type(JUMPER);
    m_state.enemies[3].set_ai_state(JUMPING);
    m_state.enemies[3].m_texture_id = jumper_texture_id;
    m_state.enemies[3].set_position(glm::vec3(11.0f, -4.0f, 0.0f));
    m_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_state.enemies[3].set_speed(1.0f);
    m_state.enemies[3].set_acceleration(glm::vec3(0.0f, -4.81f, 0.0f));
    m_state.enemies[3].set_jumping_power(2.0f);

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/audio/BlippyTrance.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/2);

    m_state.jump_sfx = Mix_LoadWAV("assets/audio/cartoon-jump.wav");
    m_state.game_won_sfx = Mix_LoadWAV("assets/audio/success-fanfare.wav");
    m_state.game_lost_sfx = Mix_LoadWAV("assets/audio/failure-drum.wav");

    Mix_VolumeChunk(m_state.jump_sfx, MIX_MAX_VOLUME / 2);
    Mix_VolumeChunk(m_state.game_won_sfx, MIX_MAX_VOLUME / 2);
    Mix_VolumeChunk(m_state.game_lost_sfx, MIX_MAX_VOLUME / 2);

}

void LevelA::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }

    
}


void LevelA::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].render(program);
    }
}