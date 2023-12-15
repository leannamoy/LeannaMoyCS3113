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

#define DRIFT_FORCE 9.0f
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
    //  delete[] m_animation_left;
    //  delete[] m_animation_right;
    //  delete[] m_walking;
}

/*
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
*/

void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {
    case SWIMMER:
        swim();
        break;

    case ALIEN:
        ai_switch(player);
        // hover(player);
        //abduct(player);
        break;

    default:
        break;
    }
}

void Entity::ai_switch(Entity* player) {
    switch (m_ai_state) {
    case WAIT:
        if (player->get_position().x > m_position.x) {
            move_right();
            moving_right = true;
            m_ai_state = HOVER;
        }
        break;
    case HOVER:
        hover(player);
        break;
    case ABDUCT:
        abduct(player);
        break;
    }
}

void Entity::swim() {
    //move back and forth
    if (m_position.x > get_fish_start_pos() + 1.5f && moving_right) {
        m_movement.x = -1.0f;
        moving_right = false;
        moving_left = true;
    }
    else if (m_position.x < get_fish_start_pos()-1.5f && moving_left) {
        moving_left = false;
        moving_right = true;
        m_movement.x = 1.0f;

    }
    else if (!moving_right && !moving_left) {
        moving_right = true;
        move_right();
    }
    
}

void Entity::hover(Entity* player) {
    //ufo circles the player
    if (m_position.x > player->get_position().x + 0.5f && moving_right) {
        m_movement.x = -1.0f;
        moving_right = false;
        moving_left = true;
    }
    else if (m_position.x < player->get_position().x - 0.5f && moving_left) {
        moving_left = false;
        moving_right = true;
        m_movement.x = 1.0f;
        
    }
    else if (!moving_right && !moving_left) {
        moving_right = true;
    }

    if (! ( - 0.25f < m_position.y - player->get_position().y + 2 < 0.25f)) {
        if (m_position.y > player->get_position().y + 2.5) {
            m_movement.y = -1.0f;
        }
        else if (m_position.y < player->get_position().y + 1.5) {
            m_movement.y = 1.0f;
        }
    }
    else { m_movement.y = 0.0f; }
    
}

void Entity::abduct(Entity* player) {
    //first, get above player
    { // then we are on top? can now abduct!
        if (attemptAbduct == false || glm::distance(m_position, player->m_position) > 0.75f) {
            attemptAbduct = true;
            m_movement.x = 0.0f;
            m_movement.y = -1.0f;
        }


        else if (attemptAbduct && glm::distance(m_position, player->m_position) < 0.75f
            && player->get_hook()->get_collided_with_type() != ROCK) { // got close enough and not hooked on rock.
                player->set_abducted(this);  // sends "this" to player so it can follow the position
                abductionDone = true;
        }

        if (player->abductedBy == this || (attemptAbduct&& abductionDone)) {
            m_movement.y = 1.0f; // go back up
            set_speed(3.0f);
        }

    }
}

void Entity::reset_hook(Entity* player) {
    m_collided_with_entity = nullptr;
    m_collided_with_type = NONE;
    set_position(glm::vec3(player->get_position().x + 1.0f,
        player->get_position().y, 0.0f));
    old_pos = m_position;
    set_isLaunched(false);
    set_reeling(false);
    set_extended(false);
    isHooked = false;
    set_speed(2.5f);
    set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
    player->set_speed(2.5f);
}

glm::vec3 Entity::getReelingDirectionVector(Entity* other) {
    //hooked onto something
    //The entity calling this function will be the hook, which will have collided with the
    //fish/obj. and the pointer from there will be used. so essentially the obj is calling this.
    float x_dir = (other->m_position.x - m_position.x > 0) ? 1 : (other->m_position.x - m_position.x < 0) ? -1 : 0;
    float y_dir = (other->m_position.y - m_position.y > 0) ? 1 : (other->m_position.y - m_position.y < 0) ? - 1 : 0;
    return glm::vec3(x_dir, y_dir, 0.0f);
}

float Entity::get_angle(Entity* ent) {
        //use angle and sin and cos to get movement 
    float x = abs(ent->get_position().x - m_position.x);
    float y = abs(ent->get_position().y - m_position.y);
    float z = sqrt(pow(x,2) + pow(y,2));

    float angle = acos(x / z);
    return angle;
}

void Entity::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map, Entity* extra_obj, int extra_obj_count)
{
    if (!m_is_active) return;

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == FISH) {
        ai_activate(player);
        if (isHooked) m_movement = glm::vec3(0.0f); // stop the fish
    }

    /*  if (m_animation_indices != NULL)
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
      */
    
    if (m_entity_type == UFO) {
        ai_activate(player);
        if (m_ai_state == HOVER) {
            m_timer -= 1;
            if (m_timer < 0) {
                set_ai_state(ABDUCT);
            }
        }
    }

    
    m_model_matrix = glm::mat4(1.0f);

    //// BLACKHOLE ENTITY STUFF ////
    if (m_entity_type == BLACKHOLE) {
        if (glm::distance(m_position, player->get_position()) < 1.5f) {
            float angle = get_angle(player);
            float x_dir = (m_position.x - player->get_position().x > 0) ? 1 : -1;
            float y_dir = (m_position.y - player->get_position().y > 0) ? 1 : -1;
            player->set_movement(glm::vec3(x_dir * cos(angle)/m_speed, y_dir*sin(angle)/m_speed, 0.0f));
           // player->set_movement(glm::vec3(x_dir, y_dir, 0.0f));

            //multiplying x and y by player speed optional?
        }
        if (glm::distance(m_position, player->get_position()) < 0.3f) {
            player->deactivate();
            player->m_model_matrix = glm::scale(player->m_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        }
        

        //sucking in aliens
        for (int i = 0; i < extra_obj_count; i++) {
            Entity* ent = &extra_obj[i];
            if (glm::distance(m_position, ent->get_position()) < 2.5f) {
                float angle = get_angle(ent);
                float x_dir = (m_position.x - ent->get_position().x > 0) ? 1 : -1;
                float y_dir = (m_position.y - ent->get_position().y > 0) ? 1 : -1;
                ent->m_movement += (glm::vec3(x_dir * cos(angle), y_dir * sin(angle), 0.0f));
            }

            if (glm::distance(m_position, ent->get_position()) < 0.8f) {
                ent->deactivate();
                ent->m_model_matrix = glm::scale(ent->m_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }
    }

    
    //HOOKING ONTO FISH. when space pressed, this may happen
    if (m_entity_type == HOOK && m_collided_with_type == FISH) {
        isHooked = true;
        m_collided_with_entity->isHooked = true;
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f);

        if (isHooked && reeling) { //reeling in
            //need to get the direction vector (resultingMovement)
            //player->resultingMovement = getReelingDirectionVector(m_collided_with_entity);
            player->m_position = get_collided_with_entity()->m_position;
          //  player->m_movement.x = 1.0f;
            m_collided_with_entity->m_model_matrix = glm::scale(m_collided_with_entity->m_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
            m_collided_with_entity->deactivate();
            reset_hook(player);
        }

    }

    //HOOKING ONTO ROCK. anchoring
    if (m_entity_type == HOOK && m_collided_with_type == ROCK && isLaunched) {
        //follow rock position, with an offset?
        isHooked = true;

        m_position = m_collided_with_entity->m_position;
        set_og_pos(m_position);
        player->m_position = glm::vec3(m_position.x - 1, m_position.y, 0.0f);

        if (isHooked && reeling) { //then detach (also deactivate rock?)
            player->m_position = get_collided_with_entity()->m_position;
            m_collided_with_entity->deactivate();
            m_collided_with_entity->m_model_matrix = glm::scale(m_collided_with_entity->m_model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
            reset_hook(player);
        }
    }

    

    //player dealing with aliens
    if (m_entity_type == PLAYER && abductedBy) {
        m_position = glm::vec3(abductedBy->get_position().x, abductedBy->get_position().y - 1.0f, 0.0f);

        if (m_position.y > 0.0f) { player->deactivate(); player->get_hook()->deactivate(); }
    }

    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;

    m_velocity.y = m_movement.y * m_speed;

    

    //if velocity more than 0, add negative acceleration
    //if velocity less than 0, add positive acceleration

    // We make two calls to our check_collision methods, one for the collidable objects and one for
    // the map.

    m_position.y += m_velocity.y * delta_time;
    if (m_entity_type == HOOK && isLaunched) {
        check_collision_y(objects, object_count);
    }
    if (extra_obj) {
        check_collision_y(extra_obj, extra_obj_count);
    }

    
    ///check_collision_y(map);

    m_position.x += m_velocity.x * delta_time;
    if (m_entity_type == HOOK && isLaunched) {
        check_collision_x(objects, object_count);
    }
    if (extra_obj) {
        check_collision_x(extra_obj, extra_obj_count);
    }
//    check_collision_x(map);


    ///// ROCK STUFF /////
    if (m_entity_type == ROCK) {
        if (m_position.y < old_pos.y-14.0f) {
            reset_rock_pos();

        }
    }


    m_model_matrix = glm::translate(m_model_matrix, m_position);

    if (m_entity_type == HOOK) {
        
        m_model_matrix = glm::rotate(m_model_matrix, m_rot_angle, glm::vec3(0.0f, 0.0f, 1.0f));
        

        if (justExtended) {
            justExtended = false;
            isLaunched = true;
            m_movement = glm::vec3(cos(m_rot_angle), sin(m_rot_angle), 0.0f);
        }


        if (isLaunched && glm::distance(m_position, old_pos) > 3.5f) { // caught nothing!
            //  then invert hook movement until returned to old_pos, then stop!
            m_movement.x = -m_movement.x;
            m_movement.y = -m_movement.y;
            isLaunched = false;
            // reeling = false;
        }
        //follow up of above, to return to og position
        else if (!isLaunched && glm::distance(m_position, old_pos) < 0.01f) {
            //"teleport" back to og pos,
            reset_hook(player);
        }
    }

    if (m_entity_type == GOAL) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(3.0f, 3.0f, 0.0f));
    }

    if (m_entity_type == PLAYER) {
        m_model_matrix = glm::scale(m_model_matrix, defaultScaleVector);
    }

    if (!m_is_active) {
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

            m_collided_with_type = collidable_entity->get_entity_type();
            m_collided_with_entity = collidable_entity;
        }
    }
}

void const Entity::check_collision_y(Map* map)
{
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
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = -m_velocity.x;

        m_collided_right = true;
    }
}

void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

   // if (m_animation_indices != NULL)
  //  {
  //      draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
   //     return;
   // }

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