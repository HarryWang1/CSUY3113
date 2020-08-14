//
//  LevelOver.cpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "LevelOver.hpp"

#define LEVELOVER_WIDTH 14
#define LEVELOVER_HEIGHT 8

unsigned int levelOver_data[] =
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

void LevelOver::Initialize()
{
    state.nextScene = -1;
    
    // Setup Platform
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVELOVER_WIDTH, LEVELOVER_HEIGHT, levelOver_data, mapTextureID, 1.0f, 4, 1);
}

void LevelOver::Update(float deltaTime)
{
    
}

void LevelOver::Render(ShaderProgram *program)
{
    state.map->Render(program);    
}

void LevelOver::Reset()
{
    LevelOver::Initialize();
}
