//
//  Level4.cpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Level4.hpp"

#define LEVEL4_ENEMY_COUNT 1
#define LEVEL4_WIDTH 14
#define LEVEL4_HEIGHT 8

unsigned int level4_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 3, 3, 3, 0, 0, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 3, 3, 3, 0, 0, 2, 2, 2
};

void Level4::Initialize()
{
    state.nextScene = -1;
    
    // Setup Player Values
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5.0, 1.0, 0.0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0, -9.81, 0.0);
    state.player->velocity = glm::vec3(0);
    state.player->speed = 0.2f;
    state.player->jumpPower = 0.25f;
    state.player->textureID = Util::LoadTexture("playerWizard.png");
    
    
    
    
    // Setup Player's Flamespin Projectile Values
    state.flamespin = new Entity();
    state.flamespin->isActive = false;
    state.flamespin->entityType = ATTACKOBJECT;
    state.flamespin->position = state.player->position;
    state.flamespin->movement = glm::vec3(1.0, 0.0, 0.0);
    state.flamespin->velocity = glm::vec3(1.0, 0.0, 0.0);
    state.flamespin->speed = 2.0f;
    state.flamespin->textureID = Util::LoadTexture("flamespinRight.png");
    
    
    
    // Setup Platform
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL4_WIDTH, LEVEL4_HEIGHT, level4_data, mapTextureID, 1.0f, 4, 1);
    
    state.player->isWin = true;
}

void Level4::Update(float deltaTime)
{
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
}

void Level4::Render(ShaderProgram *program)
{
    state.map->Render(program);
    
    state.player->Render(program); //Render Player
}

void Level4::Reset()
{
    Level4::Initialize();
}
