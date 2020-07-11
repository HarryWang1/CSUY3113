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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.hpp"

#define GROUND_COUNT 10
#define ENEMY_COUNT 3

struct GameState
{
    Entity *player;
    Entity *flamespin; //player's projectile
    Entity *fireball; //enemy witch's projectile
    Entity *ground; //platform
    Entity *enemies;
};

GameState state;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, terrainMatrix, leftMatrix;

GLuint font;

SDL_Window* displayWindow;
bool gameIsRunning = true;

int *animIndices = new int[4] {3, 7, 11, 15};
int animFrames = 4;
int animIndex = 0;

void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    int cols = 4;
    int rows = 4;
    
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;
    
    float texCoords[] = {
        u, v + height,
        u + width, v + height,
        u + width, v,
        
        u, v + height,
        u + width, v,
        u, v
    };
    
    float vertices[] = {
        -0,5, -0.5,
        0.5, -0.5,
        0.5, 0.5,
        
        -0.5, -0.5,
        0.5, 0.5,
        -0.5, 0.5
    };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for (int i = 0; i < text.size(); i++)
    {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(),
                        {
                            offset + (-0.5f * size), 0.5f * size,
                            offset + (-0.5f * size), -0.5f * size,
                            offset + (0.5f * size), 0.5f * size,
                            offset + (0.5f * size), -0.5f * size,
                            offset + (0.5f * size), 0.5f * size,
                            offset + (-0.5f * size), -0.5f * size
                        });
        
        texCoords.insert(texCoords.end(),
                         {
                             u, v,
                             u, v + height,
                             u + width, v,
                             u + width, v + height,
                             u + width, v,
                             u, v + height
                         });
    }
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

GLuint LoadTexture(const char* filePath)
{
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (!state.player->isActive) //Draw Game Over
    {
        DrawText(&program, font, "Game Over", 0.5f, -0.25f, glm::vec3(-2.0f, 1.0f, 0.0f));
    }
    else if (state.player->enemiesKilled == ENEMY_COUNT) //Draw You Win
    {
        DrawText(&program, font, "You Win", 0.5f, -0.25f, glm::vec3(-2.0f, 1.0f, 0.0f));
    }
    else //If still in game draw instructions
    {
        DrawText(&program, font, "Press and Hold Spacebar to Fly", 0.5f, -0.25f, glm::vec3(-4.0f, 3.0f, 0.0f));
        DrawText(&program, font, "Press 'F' to fire Flamespin", 0.5f, -0.25f, glm::vec3(-4.0f, 2.0f, 0.0f));
    }
    
    for (int i = 0; i < GROUND_COUNT; i++) //Renders Ground
    {
        state.ground[i].Render(&program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) //Renders Enemies
    {
        state.enemies[i].Render(&program);
    }
    
    state.player->Render(&program); //Render Player
    
    if (state.flamespin->isActive) //Render Flamespin if triggered
    {
        state.flamespin->Render(&program);
    }
    
    if (state.fireball->isActive) //Render fireball if triggered
    {
        state.fireball->Render(&program);
    }

    SDL_GL_SwapWindow(displayWindow);
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP)
    {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP && state.player->isActive) //Checks to make sure player is
                                                                // still alive to keep enemy movement
    {
        state.player->Update(FIXED_TIMESTEP, state.player, &state.enemies[state.player->enemiesKilled], state.fireball, state.ground, GROUND_COUNT);
        
        if (state.player->collidedLeft || state.player->collidedRight) //Check collision death
        {
            state.player->isActive = false;
            state.player->isWin = false;
        }
        
        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            if (state.enemies[i].isActive) //If enemy is equal to level create
            {
                state.enemies[i].Update(FIXED_TIMESTEP, state.player, &state.enemies[i], state.fireball, state.ground, GROUND_COUNT);
            }
            
            if (state.enemies[i].shootFireball) //If statement check for witch fireball
            {
                state.fireball->Update(FIXED_TIMESTEP, &state.enemies[i], state.player, state.fireball, state.ground, GROUND_COUNT);
                
                if (glm::distance(state.fireball->position, state.player->position) > 4.0) //removes fireball
                                                                                    //if out of distance
                {
                    state.fireball->isActive = false;
                    state.enemies[i].shootFireball = false;
                }
                
                if (!state.fireball->isActive && !state.player->isActive) //removes fireball if player killed
                {
                    state.enemies[i].shootFireball = false;
                }
            }
            
            if (state.flamespin->isActive) //If statement check for player flamespin
            {
                state.flamespin->Update(FIXED_TIMESTEP, state.player, &state.enemies[i], state.fireball, state.ground, GROUND_COUNT);
                
                if (glm::distance(state.flamespin->position, state.player->position) > 4.0) //removes flamespin
                                                                                        //if out of distance
                {
                    state.flamespin->isActive = false;
                    state.player->shootFlamespin = false;
                }
                
                if (!state.flamespin->isActive && !state.enemies[i].isActive)
                {
                    state.player->shootFlamespin = false; // Removes flamespin if enemy is killed by it
                }
            }
            
            state.enemies[state.player->enemiesKilled].isActive = true; //Only generates enemy of each level
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}

void ProcessInput()
{
    // Keyboard Controller
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT])
    {
        state.player->position.x += -0.1f;
    }
    if (keys[SDL_SCANCODE_RIGHT])
    {
        state.player->position.x += 0.1f;
    }
    if (keys[SDL_SCANCODE_SPACE]) // Player's Flying ability
    {
        if (!state.player->jump) state.player->jump = true;
    }
    if (keys[SDL_SCANCODE_F])
    {
        if (!state.player->shootFlamespin)
        {
            state.player->shootFlamespin = true;
            
            state.flamespin->position = state.player->position;
            
            state.flamespin->position.x = state.player->position.x + state.player->width; //sets flamespin to next
                                                                            //to player instead of on top of
            
            state.flamespin->isActive = true;
        }
    }
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    terrainMatrix = glm::mat4(1.0f);
    leftMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 4.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    font = LoadTexture("font.png");
    
    
    
    
    // Setup Player Values
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4.0, 0.0, 0.0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0, -9.81, 0.0);
    state.player->velocity = glm::vec3(0);
    state.player->speed = 1.0f;
    state.player->jumpPower = 0.25f;
    state.player->textureID = LoadTexture("playerWizard.png");
    
    
    
    
    // Setup Player's Flamespin Projectile Values
    state.flamespin = new Entity();
    state.flamespin->isActive = false;
    state.flamespin->entityType = ATTACKOBJECT;
    state.flamespin->position = state.player->position;
    state.flamespin->movement = glm::vec3(1.0, 0.0, 0.0);
    state.flamespin->velocity = glm::vec3(1.0, 0.0, 0.0);
    state.flamespin->speed = 2.0f;
    state.flamespin->textureID = LoadTexture("flamespinRight.png");
    
    
    
    // Setup Platform
    state.ground = new Entity[GROUND_COUNT];
    
    GLuint groundTextureID = LoadTexture("grass.png");

    float blockPosOffset = 0;
    
    for (int i = 0; i < GROUND_COUNT; i++)
    {
        state.ground[i].entityType = PLATFORM;
        state.ground[i].textureID = groundTextureID;
        
        state.ground[i].position = glm::vec3(-5.0f + blockPosOffset, -3.25f, 0.0f);
        
        blockPosOffset += 1.0f;
        
    }
    
    for (int i = 0; i < GROUND_COUNT; i++)
    {
        state.ground[i].Update(0, state.player, &state.enemies[state.player->enemiesKilled], state.fireball, state.ground, 0);
    }
    
    
    
    // Setup Enemies
    state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++) // Generates the 3 enemies and sets defaults
    {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = IDLE;
        state.enemies[i].position = glm::vec3(3.0, 5.0, 0.0);
        state.enemies[i].acceleration = glm::vec3(0.0, -9.81, 0.0);
        state.enemies[i].speed = 1.0;
    }
    
    state.enemies[1].isActive = false;
    state.enemies[1].position.y = 1.0;
    state.enemies[1].aiType = JUMPER;
    
    state.enemies[2].isActive = false;
    state.enemies[2].aiType = WITCH;
    
    state.enemies[0].textureID = LoadTexture("enemyPerson.png");
    state.enemies[1].textureID = LoadTexture("enemyVampire.png");
    state.enemies[2].textureID = LoadTexture("enemyWitch.png");
    
    // Setup Witch's Fireball Projectile Values
    state.fireball = new Entity();
    state.fireball->isActive = false;
    state.fireball->entityType = ATTACKOBJECT;
    state.fireball->position = state.enemies[2].position;
    state.fireball->movement = glm::vec3(1.0, 0.0, 0.0);
    state.fireball->velocity = glm::vec3(1.0, 0.0, 0.0);
    state.fireball->speed = -2.0f;
    state.fireball->textureID = LoadTexture("fireballLeft.png");
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                gameIsRunning = false;
            }
        }
        
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
