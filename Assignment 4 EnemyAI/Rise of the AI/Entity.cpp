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
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

Entity::Entity()
{
    // ––––– PHYSICS ––––– //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity()
{
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_walking;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {
    case WALKER:
        ai_walk();
        break;

    case GUARD:
        ai_guard(player);
        break;

    case SKATER:
        ai_skate(player);
        break;

    case JUMPER:
        ai_jump(player);
        break;

    default:
        break;
    }
}

void Entity::ai_walk()
{
    m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////
/*
SKATE AI
    This AI is on a skateboard;
        --Can only call 1 direction at a time, until acceleration = 0:
            skate left      OR      skate right
            based on where the player is respective to the skater
        1. Skate towards player, ACCELERATING
        2. Slow down to a stop 
*/
void Entity::ai_skate_left() {
    if (m_acceleration.x <= 0.0f) {
        m_movement.x = -1.0f;
        m_acceleration.x = 2.5f;
    }
}

void Entity::ai_skate_right() {
    if (m_acceleration.x <= 0.0f) {
        m_movement.x = 1.0f;
        m_acceleration.x = 2.5f;
    }
}

void Entity::ai_skate(Entity* player) {
    if (m_position.x < player->get_position().x) { //ai is to the left of player
        ai_skate_right();
    }
    else { //ai is to right of player
        ai_skate_left();
    }
    
    m_acceleration -= 0.01f;
}


///////////////////////////////////////////////////////////////////////

/*
(SHOOTER) SLINGSHOT AI
    This AI uses a slingshot, and is hovering so that the player cannot
    jump on them to defeat them. The only way to defeat them is to
    press the shield button (?) and deflect their projectile.
        1. Shoot another projectile after the first one hits the floor

    //add a rotation effect on projectile
*/
//Shooting, from the projectile's POV
void Entity::ai_shoot(Entity* player) {
    if (m_position.y <= -5.0f) { //if hit floor, reset 
        m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        set_position(shooter_owner->get_position());
        int run = shooter_owner->get_position().x - player->get_position().x;
        m_movement.x = -run;
        m_movement.y = -1.0f;
    }
    

}


/////////////////////////////////////////////////////////////////////////

/*
JUMPER AI
    This AI jumps up and down (slowly?) so the player must time their jumps in order to step on their head
*/
void Entity::ai_jump(Entity* player) {
    if (m_position.y <= -4.0f) {
        m_is_jumping = true;
    }
}

/////////////////////////////////////////////////////////////////////////

void Entity::ai_guard(Entity* player)
{
    switch (m_ai_state) {
    case IDLE:
        if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = WALKING;
        break;

    case WALKING:
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        else {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        break;

    case ATTACKING:
        break;

    case SKATING:
        ai_skate(player);

    case SHOOTING:
        ai_shoot(player);
    case JUMPING:
        ai_jump(player);

    default:
        break;
    }
}


void Entity::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == ENEMY) ai_activate(player);
    if (m_entity_type == PROJECTILE && shooter_owner->m_is_active) ai_shoot(player);

    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / SECONDS_PER_FRAME;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }

    if (m_entity_type != PROJECTILE) {
        m_velocity.x = m_movement.x * m_speed;
        m_velocity += m_acceleration * delta_time;
    }
    else{ // it's a projectile
        m_velocity.x = m_movement.x * m_speed;
        m_velocity.y = m_movement.y * m_speed;
        m_velocity += m_acceleration * delta_time;
    }

    if (m_ai_type == SKATER) {
        m_velocity.x = m_movement.x * m_speed * m_acceleration.x;
    }

    // We make two calls to our check_collision methods, one for the collidable objects and one for
    // the map.
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    check_collision_y(map);

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(objects, object_count);
    check_collision_x(map);

    //Checking if enemy is defeated by being jumped on:
    if (m_entity_type == ENEMY && player->get_collided_with_type() == ENEMY) {
        if (!player->m_collided_left && !player->m_collided_right
            && player->get_collided_with_entity() == this) {
            deactivate();
        }
    }

    //PROJECTILE MOVEMENTS, after collisions checked
    if (m_entity_type == PROJECTILE) {
        if (m_collided_left || m_collided_right) {
            //then invert movement.x for a ricochet effect
            m_movement.x = -m_movement.x;
        }
        //also, check position; if sent back, enemy defeated
        if (m_position.y > shooter_owner->get_position().y) {
            this->deactivate(); //deactivate this projectile as well to make it disappear
            shooter_owner->deactivate();
            shooter_owner->m_model_matrix = glm::scale(m_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }


    if (m_is_jumping)
    {
        m_is_jumping = false;

        m_velocity.y += m_jumping_power;
    }

    //player defeated by enemy
    if (m_entity_type == PLAYER && m_collided_with_type == ENEMY) {
        if (m_collided_right || m_collided_left) {
            deactivate();
        }
    }

    //checking collisions with the projectile
    if (m_entity_type == PLAYER && m_collided_with_type == PROJECTILE) {
        if (!shielding) {
            deactivate();
        }
        else {
            m_collided_with_entity->m_movement.x = -m_collided_with_entity->m_movement.x;
            m_collided_with_entity->m_movement.y = 1.0f;
            m_collided_with_type = PLATFORM;
        }
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    if (m_entity_type == PROJECTILE) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(0.25f, 0.25f, 0.0f));
    }

    if (!m_is_active && m_entity_type != PLAYER) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    }

}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }

            m_collided_with_type = collidable_entity->get_entity_type();
            m_collided_with_entity = collidable_entity;
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    if (m_entity_type == PROJECTILE) return;
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
            m_acceleration.x = 0.0f; //skater hit wall
            m_collided_with_type = collidable_entity->get_entity_type();
            m_collided_with_entity = collidable_entity;

        }
    }
}


void const Entity::check_collision_y(Map* map)
{
    if (m_entity_type == PROJECTILE) return;
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{

    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
        m_collided_with_type = PLATFORM;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
        m_collided_with_type = PLATFORM;
    }
}



void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}