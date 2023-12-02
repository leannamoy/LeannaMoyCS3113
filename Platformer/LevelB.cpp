/**
* Author: Leanna Moy
* Assignment: Platformer
* Date due: 2023-12-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 32
#define LEVEL_HEIGHT 8


unsigned int LEVELB_DATA[] =
{
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 3, 3, 3, 2, 2, 2, 2, 2, 3, 3, 3, 2, 2, 3, 1, 3, 2, 2, 2,
    1, 2, 3, 3, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 3, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2,
                                                     //17
}; //2 = bg, 1 = walls, 3 = slabs

LevelB::~LevelB()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    delete[] m_state.others;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeChunk(m_state.game_lost_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise()
{
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/images/B_tileset.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 4, 1);

    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(start_pos);
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

    
    // Other entities (i.e. springs)
    GLuint spring_texture_id = Utility::load_texture("assets/images/mushroom.png");

    m_state.others = new Entity[OTHER_COUNT];
    m_state.others[0].set_entity_type(SPRING);
    m_state.others[0].m_texture_id = spring_texture_id;
    m_state.others[0].set_position(glm::vec3(9.0f, -6.0f, 0.0f));
    m_state.others[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_state.others[1].set_entity_type(SPRING);
    m_state.others[1].m_texture_id = spring_texture_id;
    m_state.others[1].set_position(glm::vec3(19.0f, -3.0f, 0.0f));
    m_state.others[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));


    // Enemies
    GLuint enemy_texture_id = Utility::load_texture("assets/images/jumper.png");
    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_position(glm::vec3(15.0f, -1.0f, 0.0f));
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.enemies[0].set_speed(1.5f);
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_height(1.0f);
    m_state.enemies[0].set_width(0.8f);

    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].set_ai_state(IDLE);
    m_state.enemies[1].m_texture_id = enemy_texture_id;
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_position(glm::vec3(17.0f, -1.0f, 0.0f));
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.enemies[1].set_speed(1.5f);
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_height(1.0f);
    m_state.enemies[1].set_width(0.8f);

    m_state.enemies[2].set_entity_type(ENEMY);
    m_state.enemies[2].set_ai_type(GUARD);
    m_state.enemies[2].set_ai_state(IDLE);
    m_state.enemies[2].m_texture_id = enemy_texture_id;
    m_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_state.enemies[2].set_position(glm::vec3(28.0f, -1.0f, 0.0f));
    m_state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.enemies[2].set_speed(1.5f);
    m_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_state.enemies[2].set_height(1.0f);
    m_state.enemies[2].set_width(0.8f);

    // Bgm and SFX
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);


    m_state.jump_sfx = Mix_LoadWAV("assets/audio/cartoon-jump.wav");
    m_state.game_lost_sfx = Mix_LoadWAV("assets/audio/failure-drum.wav");

    Mix_VolumeChunk(m_state.jump_sfx, MIX_MAX_VOLUME / 2);
    Mix_VolumeChunk(m_state.game_lost_sfx, MIX_MAX_VOLUME / 2);

}

void LevelB::update(float delta_time, int& lives) {
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    //others i.e. springs (check for collision with player)
    for (int i = 0; i < OTHER_COUNT; i++) {
        m_state.others[i].update(delta_time, m_state.player, m_state.player, 1, m_state.map);
    }
    //enemies
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
 
    }
    if (lives-1 >= 0 && !m_state.player->get_active()) {
        m_state.player->activate();
        m_state.player->set_position(start_pos);
        lives--;
        if (lives < 0) m_state.player->deactivate();
    }

    else {
        if (m_state.player->get_position().y < -8.0f && m_state.player->get_position().x > 27.0f) m_state.next_scene_id = 3;
        else if (m_state.player->get_position().y < -8.0f) m_state.player->deactivate();
    }

}

void LevelB::render(ShaderProgram* program) {
    m_state.map->render(program);
    m_state.player->render(program);

    for (int i = 0; i < OTHER_COUNT; i++) {
        m_state.others[i].render(program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].render(program);
    }
}