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

#define ROCK_COUNT 20

struct GameState
{
    Entity *player;
    Entity *rocks;
    Entity *landing;
};

GameState state;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, terrainMatrix, leftMatrix;

GLuint font;

SDL_Window* displayWindow;
bool gameIsRunning = true;

float lastTicks = 0;

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
    glEnable(GL_BLEND);
       // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (!state.player->isActive)
    {
        if (!state.player->isWin) DrawText(&program, font, "Mission Failed!", 0.5f, -0.25f, glm::vec3(-2.0f, 1.0f, 0.0f));
        else DrawText(&program, font, "Mission Successful!", 0.5f, -0.25f, glm::vec3(-2.0f, 1.0f, 0.0f));
    }
    
    for (int i = 0; i < ROCK_COUNT; i++)
    {
        state.rocks[i].Render(&program);
    }
    
    state.landing->Render(&program);
    
    state.player->Render(&program);
    
    DrawText(&program, font, "Acceleration:" + std::to_string(state.player->acceleration.x), 0.4, -0.25f, glm::vec3(-3.5f, 3.5f, 0.0f));
    DrawText(&program, font, "Velocity:" + std::to_string(state.player->velocity.x), 0.4, -0.25f, glm::vec3(-3.5f, 3.25f, 0.0f));
    DrawText(&program, font, "Position X:" + std::to_string(state.player->position.x), 0.4, -0.25f, glm::vec3(-3.5f, 3.0f, 0.0f));
    DrawText(&program, font, "Position Y:" + std::to_string(state.player->position.y), 0.4, -0.25f, glm::vec3(-3.5f, 2.75f, 0.0f));

    SDL_GL_SwapWindow(displayWindow);
}

void Update()
{
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    state.player->Update(deltaTime, state.rocks, state.landing, ROCK_COUNT);
}

void ProcessInput()
{
    // Keyboard Controller
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->acceleration.x = 0.5f;
        state.player->rotateAngle -= 0.25f;
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->acceleration.x = -0.5f;
        state.player->rotateAngle += 0.25f;
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
    projectionMatrix = glm::ortho(-4.0f, 4.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //good for blend
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    font = LoadTexture("font.png");
    
    state.player = new Entity();
    state.player->position = glm::vec3(0.0, 2.0, 0.0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.05, 0.0);
    state.player->speed = 1.0f;
    state.player->velocity = glm::vec3(0);
    state.player->textureID = LoadTexture("player.png");
    
    //glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //don't know why won't work for blending
    
    
    state.rocks = new Entity[ROCK_COUNT];
    
    state.landing = new Entity();
    state.landing->position = glm::vec3(1.5f, -2.25f, 0.0f);
    
    GLuint enemyTextureID = LoadTexture("rock.png");
    GLuint landingTextureID = LoadTexture("landing.png");
    
    state.landing->textureID = landingTextureID;
    state.landing->width = 0.5;
    
    float blockPosOffset = 0;
    
    for (int i = 0; i < ROCK_COUNT; i++)
    {
        state.rocks[i].textureID = enemyTextureID;
        
        if (i < 9)
        {
            state.rocks[i].position = glm::vec3(-4.0f + blockPosOffset, -3.25f, 0.0f);
            
            if (i == 8) blockPosOffset = 0.0;
        }
        else if (i < 14)
        {
            state.rocks[i].position = glm::vec3(-4.5f + blockPosOffset, -2.25f, 0.0f);
        }
        else if (i < 17 && i >= 15)
        {
            state.rocks[i].position = glm::vec3(-4.5f + blockPosOffset, -2.25f, 0.0f);
            if (i == 16) blockPosOffset = 0.0;
        }
        else if (i  < 19 && i >= 15)
        {
            state.rocks[i].position = glm::vec3(-4.5f + blockPosOffset, -1.25f, 0.0f);
        }
        else
        {
            state.rocks[i].position = glm::vec3(3.5f, -1.25f, 0.0f);
        }
        
        blockPosOffset += 1.0f;
        
    }
    
    for (int i = 0; i < ROCK_COUNT; i++)
    {
        state.rocks[i].Update(0, state.rocks, state.landing, 0);
    }
    
    state.landing->Update(0, state.rocks, state.landing, 0);
    
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
