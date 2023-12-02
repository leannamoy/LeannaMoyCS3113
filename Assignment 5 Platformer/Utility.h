#pragma once
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Utility {
public:
    // ����� METHODS ����� //
    static GLuint load_texture(const char* filepath);
    static void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position);
    static void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id, ShaderProgram* program, glm::vec3 position);
};