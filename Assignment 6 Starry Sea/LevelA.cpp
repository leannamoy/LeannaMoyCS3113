/**
* Author: Leanna Moy
* Assignment: Sea of Stars
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"
#include <tuple>

#define LevelA_WIDTH 32
#define LevelA_HEIGHT 12

unsigned int LevelA_DATA[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

LevelA::~LevelA() {
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

void LevelA::initialise() {
    m_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/images/star bg.png");


    m_state.map = new Map(LevelA_WIDTH, LevelA_HEIGHT, LevelA_DATA, map_texture_id, 1.0f, 1, 1);
    
    
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
    m_state.goal->m_texture_id = Utility::load_texture("assets/images/wormhole.png");

    float x;
    float y;

  //////////////////////////////////////// INITIALIZING ENEMIES ////////////////////////////////////////
    ///// FISH /////
    m_state.fish = new Entity[FISH_COUNT];
    //make a vector with fish position floats
    std::vector<std::tuple<float, float>> fish_pos;
    fish_pos.push_back(std::make_tuple(5 + 3.0f, -5.0f));
    fish_pos.push_back(std::make_tuple(5 + 6.0f, -5.0f + 3.0f));
    fish_pos.push_back(std::make_tuple(13.3f, -7.7f));
    fish_pos.push_back(std::make_tuple(12.4f, -5.0f));
    fish_pos.push_back(std::make_tuple(17.0f, -6.22f));
    for (int i = 0; i < FISH_COUNT; i++) {
        x = std::get<0>(fish_pos[i]);
        y = std::get<1>(fish_pos[i]);
        m_state.fish[i].set_entity_type(FISH);
        m_state.fish[i].set_ai_type(SWIMMER);
        m_state.fish[i].set_position(glm::vec3(x,y,0.0f));
        m_state.fish[i].set_fish_start_pos_x(x);
        m_state.fish[i].set_movement(glm::vec3(0.0f));
        m_state.fish[i].set_speed(0.75f);
        m_state.fish[i].m_texture_id = Utility::load_texture("assets/images/slime fish.png");
    }

    /////// ROCKS /////
    m_state.rocks = new Entity[ROCK_COUNT];
    std::vector<std::tuple<float, float>> rock_pos;
    rock_pos.push_back(std::make_tuple(15.9f, -2.0f)); ////////////need to change the entity.cpp file bc it resets based on position
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

    //blackholes not collidable by hook so dont add this to collidable_objects

    /////// BLACK HOLES //////
    m_state.blackholes = new Entity[BLACKHOLE_COUNT];
    std::vector<std::tuple<float, float>> blackhole_pos;
    blackhole_pos.push_back(std::make_tuple(8.8f,-1.88f));
    for (int i = 0; i < BLACKHOLE_COUNT; i++) {
        x = std::get<0>(blackhole_pos[i]);
        y = std::get<1>(blackhole_pos[i]);
        m_state.blackholes[i].set_entity_type(BLACKHOLE);
        m_state.blackholes[i].set_position(glm::vec3(x, y, 0.0f));
        m_state.blackholes[i].set_speed(1.5f);
        m_state.blackholes[i].m_texture_id = Utility::load_texture("assets/images/blackhole.png");
    }


    /////// UFOS ////////
    m_state.ufos = new Entity[UFO_COUNT];
    std::vector<std::tuple<float, float>> ufo_pos;
    ufo_pos.push_back(std::make_tuple(12.0f, 0.0f));
    for (int i = 0; i < UFO_COUNT; i++) {
        x = std::get<0>(ufo_pos[i]);
        y = std::get<1>(ufo_pos[i]);
        m_state.ufos[i].set_entity_type(UFO);
        m_state.ufos[i].set_ai_type(ALIEN);
        m_state.ufos[i].set_ai_state(WAIT);
        m_state.ufos[i].set_speed(1.5f);
        m_state.ufos[i].set_timer(250);
        m_state.ufos[i].set_movement(glm::vec3(0.0f));
        m_state.ufos[i].set_position(glm::vec3(x,y, 0.0f));
        m_state.ufos[i].m_texture_id = Utility::load_texture("assets/images/alien.png");
    }

    //BGM and sfx
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("assets/audio/reflected-light-147979.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    m_state.hook_sfx = Mix_LoadWAV("assets/audio/hook.wav");
}

void LevelA::update(float delta_time)
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

    if (goal_pos.x - m_state.player->get_position().x < 4.0) {
        m_state.player->set_acceleration(glm::vec3(2.0f, 0.0f, 0.0f));
        m_state.player->move_right();
    }

    if (m_state.player->get_position().x > goal_pos.x-0.5f) {
        m_state.next_scene_id = 2; //change this later on!
    }
}

void LevelA::render(ShaderProgram* program)
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

    Utility::draw_text(program, text_texture_id, std::string("<- and -> to rotate hook,"), 0.25f, 0.0f, glm::vec3(5.0f+-4.0f, -5.0f+-1.0f, 0.0f));
    Utility::draw_text(program, text_texture_id, std::string("and SPACE to launch!"), 0.25f, 0.0f, glm::vec3(5.0f+-4.0f, -5.0f+-1.25f, 0.0f));

    Utility::draw_text(program, text_texture_id, std::string("Avoid these black holes!"), 0.25f, 0.0f, glm::vec3(1.5f, -1.88f, 0.0f));


    Utility::draw_text(program, text_texture_id, std::string("Hook onto rocks to avoid,"), 0.25f, 0.0f, glm::vec3(5.0f + 3.0f, -5.0f + -1.0f, 0.0f));
    Utility::draw_text(program, text_texture_id, std::string("being sucked into blackholes"), 0.25f, 0.0f, glm::vec3(5.0f + 3.0f, -5.0f + -1.25f, 0.0f));
    Utility::draw_text(program, text_texture_id, std::string("or abducted by the UFO!"), 0.25f, 0.0f, glm::vec3(5.0f + 3.0f, -5.0f + -1.50f, 0.0f));

}