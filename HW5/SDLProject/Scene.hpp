//
//  Scene.hpp
//  Project 3
//
//  Created by Ethan Yao on 4/19/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Scene_hpp
#define Scene_hpp

#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <stdio.h>

#include "Util.hpp"
#include "Entity.hpp"
#include "Map.hpp"

struct GameState
{
    Map *map;
    Entity *player;
    Entity *flamespin; //player's projectile
    Entity *fireball; //enemy witch's projectile
    Entity *enemies;
    int nextScene;
};

class Scene
{
public:
    GameState state;
    virtual void Initialize() = 0;
    virtual void Update (float deltaTime) = 0;
    virtual void Render (ShaderProgram *program) = 0;
    virtual void Reset () = 0;
};

#endif /* Scene_hpp */
