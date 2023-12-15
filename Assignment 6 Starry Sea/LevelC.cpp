/**
* Author: Leanna Moy
* Assignment: Sea of Stars
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelC.h"
#include "Utility.h"
#include <tuple>

#define LevelC_WIDTH 70
#define LevelC_HEIGHT 12

unsigned int LevelC_DATA[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

LevelC::~LevelC() {
    delete[] m_state.fish;
    delete[] m_state.rocks;
    delete[] m_state.blackholes;
    delete[] m_state.ufos;
    delete m_state.goal;
    delete m_state.player;
    delete m_state.map;
    Mix_FreeMusic(m_state.bgm);
    Mix_FreeChunk(m_state.hook_sfx);
}

void LevelC::initialise() {
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/images/star bg.png");


    m_state.map = new Map(LevelC_WIDTH, LevelC_HEIGHT, LevelC_DATA, map_texture_id, 1.0f, 1, 1);


    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(5.0f, -5.0f, 0.0f)); //weird thing: i should  set this manually instead of using start_pos, which might just be useless?
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(1.5f);
    m_state.player->set_acceleration(glm::vec3(2.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/images/astrocat.png");

    m_state.hook = new Entity();
    m_state.hook->set_entity_type(HOOK);
    m_state.hook->set_position(start_pos); // start behind ? player
    m_state.hook->set_movement(glm::vec3(0.0f));
    m_state.hook->set_speed(3.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    ///acceleration??
    m_state.hook->m_texture_id = Utility::load_texture("assets/images/hook.png");
    m_state.player->set_hook(m_state.hook);



    //Create the portal to next stage, counts as goal!!
    m_state.goal = new Entity();
    m_state.goal->set_entity_type(GOAL);
    m_state.goal->set_position(goal_pos);
    m_state.goal->set_movement(glm::vec3(0.0f));
    m_state.goal->m_texture_id = Utility::load_texture("assets/images/rocket.png");

    float x;
    float y;

    //////////////////////////////////////// INITIALIZING ENEMIES ////////////////////////////////////////
      ///// FISH /////
    m_state.fish = new Entity[FISH_COUNT];
    //make a vector with fish position floats
    std::vector<std::tuple<float, float>> fish_pos;
    fish_pos.push_back(std::make_tuple(7.0f, -4.0f));
    fish_pos.push_back(std::make_tuple(12.0f, -7.77f));
    fish_pos.push_back(std::make_tuple(14.29f, -6.14f));
    fish_pos.push_back(std::make_tuple(26.0f, -5.2f));
    fish_pos.push_back(std::make_tuple(27.3f, -3.07f));
    fish_pos.push_back(std::make_tuple(31.5f, -4.3f));
    fish_pos.push_back(std::make_tuple(35.8f, -3.28));
    fish_pos.push_back(std::make_tuple(38.8f, -3.28));

    fish_pos.push_back(std::make_tuple(41.6f, -5.31));
    fish_pos.push_back(std::make_tuple(44.6f, -3.26));
    fish_pos.push_back(std::make_tuple(44.6f, -5.96));
    fish_pos.push_back(std::make_tuple(48.4f, -5.3f));

    for (int i = 0; i < FISH_COUNT; i++) {
        x = std::get<0>(fish_pos[i]);
        y = std::get<1>(fish_pos[i]);
        m_state.fish[i].set_entity_type(FISH);
        m_state.fish[i].set_ai_type(SWIMMER);
        m_state.fish[i].set_position(glm::vec3(x, y, 0.0f));
        m_state.fish[i].set_fish_start_pos_x(x);
        m_state.fish[i].set_movement(glm::vec3(0.0f));
        m_state.fish[i].set_speed(0.75f);
        m_state.fish[i].m_texture_id = Utility::load_texture("assets/images/slime fish.png");
    }

    /////// ROCKS /////
    m_state.rocks = new Entity[ROCK_COUNT];
    std::vector<std::tuple<float, float>> rock_pos;
    rock_pos.push_back(std::make_tuple(11.0f, -3.0f));
    rock_pos.push_back(std::make_tuple(20.01f, -1.8f));
    rock_pos.push_back(std::make_tuple(22.51f, -0.50f));
    rock_pos.push_back(std::make_tuple(25.01f, 1.0f));
    rock_pos.push_back(std::make_tuple(36.0f, 2.0f));
    rock_pos.push_back(std::make_tuple(52.0f, -2.0f));

    for (int i = 0; i < ROCK_COUNT; i++) {
        x = std::get<0>(rock_pos[i]);
        y = std::get<1>(rock_pos[i]);
        m_state.rocks[i].set_entity_type(ROCK);
        m_state.rocks[i].set_position(glm::vec3(x, y, 0.0f));
        m_state.rocks[i].set_acceleration(glm::vec3(3.0f, -2.8f, 0.0f));
        m_state.rocks[i].set_movement(glm::vec3(-1.0f, -1.0f, 0.0f));
        m_state.rocks[i].set_speed(0.25f);
        m_state.rocks[i].set_og_pos(glm::vec3(x, y, 0.0f));
        m_state.rocks[i].m_texture_id = Utility::load_texture("assets/images/rock.png");
    }
    m_state.rocks[0].set_speed(0.75f);
    m_state.rocks[5].set_speed(1.0f);

    //blackholes not collidable by hook so dont add this to collidable_objects

    /////// BLACK HOLES //////
    m_state.blackholes = new Entity[BLACKHOLE_COUNT];
    std::vector<std::tuple<float, float>> blackhole_pos;
    blackhole_pos.push_back(std::make_tuple(10.16f, -5.0f));
    blackhole_pos.push_back(std::make_tuple(17.01f, -3.8f));
    blackhole_pos.push_back(std::make_tuple(17.01f, -7.6f));
    blackhole_pos.push_back(std::make_tuple(18.44f, -2.94f));
    blackhole_pos.push_back(std::make_tuple(18.99f, -7.32f));
    blackhole_pos.push_back(std::make_tuple(48.3f, -3.28));

    for (int i = 0; i < BLACKHOLE_COUNT; i++) {
        x = std::get<0>(blackhole_pos[i]);
        y = std::get<1>(blackhole_pos[i]);
        m_state.blackholes[i].set_entity_type(BLACKHOLE);
        m_state.blackholes[i].set_position(glm::vec3(x, y, 0.0f));
        m_state.blackholes[i].set_speed(1.0f);
        m_state.blackholes[i].m_texture_id = Utility::load_texture("assets/images/blackhole.png");
    }


    /////// UFOS ////////
    m_state.ufos = new Entity[UFO_COUNT];
    std::vector<std::tuple<float, float>> ufo_pos;
    ufo_pos.push_back(std::make_tuple(13.7f, 1.3f));
    ufo_pos.push_back(std::make_tuple(14.2f, 1.6f));
    ufo_pos.push_back(std::make_tuple(14.5f, 1.9f));
    ufo_pos.push_back(std::make_tuple(26.5f, 1.9f));
   // ufo_pos.push_back(std::make_tuple(14.8f, 1.0f));


    for (int i = 0; i < UFO_COUNT; i++) {
        x = std::get<0>(ufo_pos[i]);
        y = std::get<1>(ufo_pos[i]);
        m_state.ufos[i].set_entity_type(UFO);
        m_state.ufos[i].set_ai_type(ALIEN);
        m_state.ufos[i].set_ai_state(WAIT);
        m_state.ufos[i].set_speed(1.5f);
        m_state.ufos[i].set_timer(500);
        m_state.ufos[i].set_movement(glm::vec3(0.0f));
        m_state.ufos[i].set_position(glm::vec3(x, y, 0.0f));
        m_state.ufos[i].m_texture_id = Utility::load_texture("assets/images/alien.png");
    }
    m_state.ufos[3].set_timer(300);

    //BGM and sfx
    Mix_CloseAudio();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/audio/stardust-danijel-zambo-main-version-03-13-1372.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    m_state.hook_sfx = Mix_LoadWAV("assets/audio/hook.wav");
}

void LevelC::update(float delta_time)
{
    //by adding the ufos as an extra parameter to player's update, they can get sucked in by black holes as well
    for (int i = 0; i < BLACKHOLE_COUNT; i++)
    {
        m_state.blackholes[i].update(delta_time, m_state.player, 0, 0, m_state.map, m_state.ufos, UFO_COUNT);
    }
    m_state.goal->update(delta_time, m_state.player, 0, 0, m_state.map);
    m_state.player->update(delta_time, m_state.player, m_state.fish, FISH_COUNT, m_state.map);

    for (int i = 0; i < FISH_COUNT; i++)
    {
        m_state.fish[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }

    for (int i = 0; i < ROCK_COUNT; i++)
    {
        m_state.rocks[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }


    for (int i = 0; i < UFO_COUNT; i++)
    {
        m_state.ufos[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }

    m_state.hook->update(delta_time, m_state.player, m_state.fish, FISH_COUNT, m_state.map, m_state.rocks, ROCK_COUNT);

    if (goal_pos.x - m_state.hook->get_position().x < 0.5) {
        m_state.next_scene_id = 4; // to win screen
    }

    if (m_state.player->get_position().x > goal_pos.x - 2.5) {
        m_state.next_scene_id = 1; //change this later on!
    }

    //reset
   // if (m_state.player->get_active() == false) {
   //     m_state.next_scene_id = 3;
   // }
}

void LevelC::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.goal->render(program);
    m_state.player->render(program);
    m_state.hook->render(program);
    for (int i = 0; i < FISH_COUNT; i++)
    {
        m_state.fish[i].render(program);
    }
    for (int i = 0; i < ROCK_COUNT; i++)
    {
        m_state.rocks[i].render(program);
    }
    for (int i = 0; i < BLACKHOLE_COUNT; i++)
    {
        m_state.blackholes[i].render(program);
    }
    for (int i = 0; i < UFO_COUNT; i++)
    {
        m_state.ufos[i].render(program);
    }

}