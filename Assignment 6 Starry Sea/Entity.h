#pragma once
#include "Map.h"

enum EntityType { PLAYER, FISH, GOAL, UFO, ROCK, BLACKHOLE, HOOK, NONE};
enum AIType { SWIMMER, ALIEN };
enum AIState { SWIM, HOVER, ABDUCT, WAIT};

class Entity {
private:
	bool m_is_active = true;

	// Animation - need to add later

	// Physics (gravity)
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_acceleration; // there will be horizontal but not vertical acceleration

	// Transformations
	float     m_speed;
	glm::vec3 m_movement;
	glm::mat4 m_model_matrix;

	// Enemy AI
	EntityType m_entity_type;
	AIType		m_ai_type;
	AIState		m_ai_state;
	int m_timer = 500;

	float fish_start_pos;
	Entity* abductedBy = nullptr;

	bool attemptAbduct = false;
	bool abductionDone = false;

	float m_width = 1;
	float m_height = 1;

	//HOOK STUFF
	// Stuff for hook
	//ROTATIONS
	const float ROT_SPEED = 5.0f;
	float g_angle = 0.0f;
	float m_rot_angle = 0.0f;
	glm::vec3 old_pos;
	//Bools
	bool isHooked = false;
	bool isLaunched = false;
	bool justExtended = false;
	bool reeling = false;
	glm::vec3 resultingMovement;
	glm::vec3 defaultScaleVector = glm::vec3(1.0f, 1.0f, 1.0f);


	// Player's Hook
	bool pulledForward;
	Entity* m_hook; // initialise one of these with the player
public:
	//need to add animation stuff later

	// Physics (collisions)
	bool m_collided_top = false;
	bool m_collided_bottom = false;
	bool m_collided_left = false;
	bool m_collided_right = false;

	EntityType m_collided_with_type;
	Entity* m_collided_with_entity;

	
	GLuint    m_texture_id;
	Entity* hook;

	// Methods
	Entity();
	~Entity();

	void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
	void update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map, Entity* extra_obj = nullptr, int extra_obj_count = 0); // Now, update should check for both objects in the game AND the map
	void render(ShaderProgram* program);

	bool const check_collision(Entity* other) const;
	void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
	void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);

	// Overloading our methods to check for only the map
	void const check_collision_y(Map* map);
	void const check_collision_x(Map* map);

	void move_left() { m_movement.x = -1.0f; };
	void move_right() { m_movement.x = 1.0f; };
	void move_up() { m_movement.y = 1.0f; };
	void move_down() { m_movement.y = -1.0f; };

	glm::vec3 getReelingDirectionVector(Entity* other);
	//-0.065f
	void rotate_left() { m_rot_angle += glm::radians(0.5f)*ROT_SPEED; if (m_rot_angle > 6.305005) m_rot_angle = 0; };
	void rotate_right() { m_rot_angle -= glm::radians(0.5f) * ROT_SPEED; if (m_rot_angle < -6.305005) m_rot_angle = 0; };
	void reset_hook(Entity* player);

	float get_angle(Entity* ent);


	void reset_rock_pos() { m_position = old_pos; };

	void ai_activate(Entity* player);
	void ai_switch(Entity* player);

	//add other AI movement methods below
	void swim();
	void hover(Entity* player);
	void abduct(Entity* player);
	bool moving_left = false;
	bool moving_right = false;
	bool above_player = false;

	void activate() { m_is_active = true; };
	void deactivate() { m_is_active = false; };

	// Getters
		Entity * get_collided_with_entity()     const { return m_collided_with_entity; }
	EntityType const get_entity_type()    const { return m_entity_type; };
	EntityType const get_collided_with_type() const { return m_collided_with_type; };
	AIType     const get_ai_type()        const { return m_ai_type; };
	AIState    const get_ai_state()       const { return m_ai_state; };
	glm::vec3  const get_position()       const { return m_position; };
	glm::vec3  const get_movement()       const { return m_movement; };
	glm::vec3  const get_velocity()       const { return m_velocity; };
	glm::vec3  const get_acceleration()   const { return m_acceleration; };
	float      const get_speed()          const { return m_speed; };
	int        const get_width()          const { return m_width; };
	int        const get_height()         const { return m_height; };
	bool       const get_active()         const { return m_is_active; };
	bool	   const get_isHooked()			const { return isHooked; };
	bool		const get_isLaunched()		const { return isLaunched; };
	float		const get_rot_angle()			const { return m_rot_angle; };
	bool		const get_abducted()		const { return abductedBy != nullptr; };
	float		const get_fish_start_pos()	const { return fish_start_pos; };
	glm::vec3	const get_og_pos()			const { return old_pos; };
	bool		const get_pulledForward()	const { return pulledForward; };
	Entity* get_hook()				const { return m_hook; };

	// Setters
	void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
	void const set_ai_type(AIType new_ai_type) { m_ai_type = new_ai_type; };
	void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
	void const set_position(glm::vec3 new_position) { m_position = new_position; };
	void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
	void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
	void const set_speed(float new_speed) { m_speed = new_speed; };
	void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; };
	void const set_width(float new_width) { m_width = new_width; };
	void const set_height(float new_height) { m_height = new_height; };
	void const set_hook(Entity* hook) { m_hook = hook; };
	void const set_isLaunched(bool val) { isLaunched = val; };
	void const set_og_pos(glm::vec3 pos) { old_pos = pos; };
	void const set_reeling(bool val) { reeling = val; };
	void const set_extended(bool val) { justExtended = val; }
	void const set_abducted(Entity* ent) { abductedBy = ent; };
	void const set_fish_start_pos_x(float x) { fish_start_pos = x; };
	void const set_pulledForward(bool val) { pulledForward = val; };
	void const set_timer(int val) { m_timer = val; };
	void const set_scale(float val) { defaultScaleVector = glm::vec3(val, val, 0.0f); };
};
