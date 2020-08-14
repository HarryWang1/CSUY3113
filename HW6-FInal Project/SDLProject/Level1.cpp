

#include "Level1.hpp"

#define LEVEL1_ENEMY_COUNT 4
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 18

unsigned int level1_data[] =
{
    0, 0, 0, 8, 5, 5, 5, 5, 5, 5, 9, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 3, 0, 0, 0,
    8, 5, 5, 1, 1, 1, 1, 0, 1, 1, 1, 5, 5, 9,
    2, 1, 1, 1, 10, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 10, 1, 1, 1, 3,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    2, 1, 0, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 3,
    7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6
};

void Level1::Initialize()
{
    state.nextScene = -1;
    
    // Setup Player Values
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5.0, -6.0, 0.0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0, 0.0, 0.0);
    state.player->velocity = glm::vec3(0.05, 0.05, 0.0);
    state.player->speed = 0.2f;
    state.player->jumpPower = 0.25f;
    state.player->textureID = Util::LoadTexture("linkMoveRight.png");
    
    
    // Setup Platform
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 11, 1);
    
    
    // Setup Enemies
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++)
    {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = WALKING;
        state.enemies[i].position = glm::vec3(5.0, -15.0, 0.0);
        state.enemies[i].acceleration = glm::vec3(0.0, 0.0, 0.0);
        state.enemies[i].velocity = glm::vec3(0.0, 0.0, 0.0);
        state.enemies[i].speed = 1.0;
        state.enemies[i].isActive = true;
        state.enemies[i].textureID = Util::LoadTexture("slime.png");
    }
    
    state.enemies[1].position = glm::vec3(0.0, -12.0, 0.0);
    state.enemies[2].position = glm::vec3(7.0, -7.0, 0.0);
    state.enemies[3].position = glm::vec3(10.0, -13.0, 0.0);
}

void Level1::Update(float deltaTime)
{
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++)
    {
        state.enemies[i].Update(deltaTime, state.player, &state.enemies[i], 0, state.map);
    }
}

void Level1::Render(ShaderProgram *program)
{
    if (state.player->enemiesKilled != 4 || state.player->lives != 0) state.map->Render(program);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++)
    {
        state.enemies[i].Render(program);
    }
    
    state.player->Render(program); //Render Player
}

void Level1::Reset()
{
    Level1::Initialize();
}
