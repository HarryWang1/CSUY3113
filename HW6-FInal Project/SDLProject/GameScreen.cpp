

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

