//
//  Level3.cpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Level3.hpp"

#define LEVEL3_ENEMY_COUNT 1
#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
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

void Level3::Initialize()
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
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    
    
    // Setup Enemies
    state.enemies = new Entity;
    state.enemies->entityType = ENEMY;
    state.enemies->aiType = WALKER;
    state.enemies->aiState = IDLE;
    state.enemies->position = glm::vec3(13.0, 1.0, 0.0);
    state.enemies->acceleration = glm::vec3(0.0, -9.81, 0.0);
    state.enemies->speed = 1.0;
    
    state.enemies->isActive = true;
    
    state.enemies->textureID = Util::LoadTexture("enemyVampire.png");
}

void Level3::Update(float deltaTime)
{
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
    
    state.enemies->Update(deltaTime, state.player, state.enemies, 0, state.map);
    
    if (state.enemies->isActive == false)
    {
        state.player->shootFlamespin = false;
        state.flamespin->isActive = false;
        
        state.nextScene = 3;
    }
    
    if (state.flamespin->isActive) //If statement check for player flamespin
    {
        state.flamespin->Update(deltaTime, state.player, state.enemies, state.fireball, state.map);
        
        if (glm::distance(state.flamespin->position, state.player->position) > 2.0) //removes flamespin
            //if out of distance
        {
            state.flamespin->isActive = false;
            state.player->shootFlamespin = false;
        }
        
        if (!state.flamespin->isActive && !state.enemies->isActive)
        {
            state.player->shootFlamespin = false; // Removes flamespin if enemy is killed by it
        }
    }
}

void Level3::Render(ShaderProgram *program)
{
    state.map->Render(program);
    
    state.enemies->Render(program);
    
    state.player->Render(program); //Render Player
    
    if (state.flamespin->isActive) //Render Flamespin if triggered
    {
        state.flamespin->Render(program);
    }
}

void Level3::Reset()
{
    Level3::Initialize();
}
