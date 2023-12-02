/**
* Author: Leanna Moy
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 20

unsigned int LEVELC_DATA[] =
{                                     //11
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, //top
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 3, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 1, 1, 1, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, //floor
    1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, //middle; hole here
    1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 2, 2, 2, 2, 2, 1, //floor
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,
    1, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, //chest at left end
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, // bottom, //y = -20.0f
};
//1 = dirt block (floor)
//2 = flat blue block (bg)
//3 = dirt slab
//4 = water surface

LevelC::~LevelC() {
    delete[] m_state.enemies;
    delete m_state.player;
    delete m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeChunk(m_state.game_lost_sfx);
    Mix_FreeChunk(m_state.game_won_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelC::initialise() {
    m_state.next_scene_id = -1;
    //red, blue, green, opacity
    glClearColor(0.200, 0.200, 0.200, 1.00);

    float water_accel = -2.95f;

    GLuint map_texture_id = Utility::load_texture("assets/images/C_tileset.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 4, 1);

    //Creating player
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(start_pos);
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(1.75f); //a bit slower bc underwater //1.75f
    m_state.player->set_acceleration(glm::vec3(0.0f, water_accel, 0.0f)); //less bc underwater
    m_state.player->m_texture_id = Utility::load_texture("assets/images/player_sprite.png");

    //Walking
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

    //Creating goal
    m_state.goal = new Entity();
    m_state.goal->set_entity_type(GOAL);
    m_state.goal->set_position(glm::vec3(2.0f, -18.0f, 0.0f));
    m_state.goal->set_movement(glm::vec3(0.0f));
    m_state.goal->set_speed(1.0f);
    m_state.goal->set_acceleration(glm::vec3(0.0f, water_accel, 0.0f));
    m_state.goal->m_texture_id = Utility::load_texture("assets/images/goal.png");
    
  
    

    //Enemies stuff

    GLuint enemy_texture_id = Utility::load_texture("assets/images/jumper.png");
    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_position(glm::vec3(12.0f, -5.0f, 0.0f));
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, water_accel, 0.0f));
    m_state.enemies[0].set_speed(1.0f);
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_height(1.0f);
    m_state.enemies[0].set_width(0.8f);

    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].set_ai_state(IDLE);
    m_state.enemies[1].m_texture_id = enemy_texture_id;
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_position(glm::vec3(10.94f, -8.1f, 0.0f));
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, water_accel, 0.0f));
    m_state.enemies[1].set_speed(1.0f);
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_height(1.0f);
    m_state.enemies[1].set_width(0.8f);

    m_state.enemies[2].set_entity_type(ENEMY);
    m_state.enemies[2].set_ai_type(GUARD);
    m_state.enemies[2].set_ai_state(IDLE);
    m_state.enemies[2].m_texture_id = enemy_texture_id;
    m_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_state.enemies[2].set_position(glm::vec3(3.316f, -13.0f, 0.0f));
    m_state.enemies[2].set_acceleration(glm::vec3(0.0f, water_accel, 0.0f));
    m_state.enemies[2].set_speed(1.0f);
    m_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_state.enemies[2].set_height(1.0f);
    m_state.enemies[2].set_width(0.8f);

    m_state.enemies[3].set_entity_type(ENEMY);
    m_state.enemies[3].set_ai_type(GUARD);
    m_state.enemies[3].set_ai_state(IDLE);
    m_state.enemies[3].m_texture_id = enemy_texture_id;
    m_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_state.enemies[3].set_position(glm::vec3(6.433f, -18.0f, 0.0f));
    m_state.enemies[3].set_acceleration(glm::vec3(0.0f, water_accel, 0.0f));
    m_state.enemies[3].set_speed(1.0f);
    m_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_state.enemies[3].set_height(1.0f);
    m_state.enemies[3].set_width(0.8f);

    m_state.enemies[4].set_entity_type(ENEMY);
    m_state.enemies[4].set_ai_type(GUARD);
    m_state.enemies[4].set_ai_state(IDLE);
    m_state.enemies[4].m_texture_id = enemy_texture_id;
    m_state.enemies[4].set_movement(glm::vec3(0.0f));
    m_state.enemies[4].set_position(glm::vec3(12.1f, -13.0f, 0.0f));
    m_state.enemies[4].set_acceleration(glm::vec3(0.0f, water_accel, 0.0f));
    m_state.enemies[4].set_speed(1.0f);
    m_state.enemies[4].set_movement(glm::vec3(0.0f));
    m_state.enemies[4].set_height(1.0f);
    m_state.enemies[4].set_width(0.8f);

    //BGM and SFX
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.jump_sfx = Mix_LoadWAV("assets/audio/cartoon-jump.wav");
    m_state.game_lost_sfx = Mix_LoadWAV("assets/audio/failure-drum.wav");
    m_state.game_won_sfx = Mix_LoadWAV("assets/audio/success-fanfare.wav");

    Mix_VolumeChunk(m_state.jump_sfx, MIX_MAX_VOLUME / 2);
    Mix_VolumeChunk(m_state.game_won_sfx, MIX_MAX_VOLUME / 2);
    Mix_VolumeChunk(m_state.game_lost_sfx, MIX_MAX_VOLUME / 2);
}

void LevelC::update(float delta_time, int& lives) {
    //Jumping
    m_state.player->m_jumping_power = 3.0f; //less bc underwater?

    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    m_state.goal->update(delta_time, m_state.player, m_state.player, 1, m_state.map); // check if goal is touched by player
    if (m_state.goal->get_collided_with_type() == PLAYER) {
        goal_touched = true;
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }

    //fell
    
    //if there are extra lives, the player may respawn
    if (lives > 0 && !m_state.player->get_active()) {
        m_state.player->activate();
        m_state.player->set_position(start_pos);
        lives--;
        if (lives < 0) m_state.player->deactivate();
    }
   
    else {
        if (m_state.player->get_position().y < -21.0f) m_state.player->deactivate();
    }

}

void LevelC::render(ShaderProgram* program) {
    m_state.map->render(program);
    m_state.player->render(program);
    m_state.goal->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].render(program);
    }
}