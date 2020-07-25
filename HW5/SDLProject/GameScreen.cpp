//
//  GameScreen.cpp
//  Project 3
//
//  Created by Ethan Yao on 4/20/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "GameScreen.hpp"


void GameScreen::Initialize()
{
    state.nextScene = -1;
}

void GameScreen::Update(float deltaTime)
{
    std::cout << "Hello" << std::endl;
}

void GameScreen::Render(ShaderProgram *program)
{
    std::cout << "Hello" << std::endl;
}

