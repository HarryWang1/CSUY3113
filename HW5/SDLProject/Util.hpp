//
//  Util.hpp
//  Project 3
//
//  Created by Ethan Yao on 4/19/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Util_hpp
#define Util_hpp

#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <stdio.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Util
{
public:
    static GLuint LoadTexture(const char* filePath);
    static void DrawText(ShaderProgram *program, int fontTextureID, std::string text,
                         float size, float spacing, glm::vec3 position);
};


#endif /* Util_hpp */
