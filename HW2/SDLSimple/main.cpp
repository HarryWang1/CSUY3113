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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, rightMatrix, leftMatrix;

SDL_Window* displayWindow;
bool gameIsRunning = true;

float rightPosY = 0.0f;
float leftPosY = 0.0f;

float ballPosX = 0.0f;
float ballPosY = 0.0f;

float ballSpeedX = 0.05f;
float ballSpeedY = 0.05f;


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    rightMatrix = glm::mat4(1.0f);
    leftMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-4.0f, 4.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

        glClear(GL_COLOR_BUFFER_BIT);
        program.SetProjectionMatrix(projectionMatrix);
        program.SetViewMatrix(viewMatrix);


        // Keyboard Controller
        const Uint8* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_W] && leftPosY <= 3.25f) {
            leftPosY += 0.1f;
        }
        if (keys[SDL_SCANCODE_S] && leftPosY >= -3.25f) {
            leftPosY -= 0.1f;
        }
        if (keys[SDL_SCANCODE_UP] && rightPosY <= 3.25f) {
            rightPosY += 0.1f;
        }
        if (keys[SDL_SCANCODE_DOWN] && rightPosY >= -3.25f) {
            rightPosY -= 0.1f;
        }

        // Right Bar Controller
        rightMatrix = glm::mat4(1.0f);
        rightMatrix = glm::translate(rightMatrix, glm::vec3(0.0f, rightPosY, 0.0f));
        program.SetModelMatrix(rightMatrix);

        float right[] = { 4.0f, 0.0f, 3.9f, 1.0f, 4.0f, 1.0f, 3.9f, 1.0f, 4.0f, 0.0f, 3.9f, 0.0f };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, right);
        glEnableVertexAttribArray(program.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        // Left Bar controller
        leftMatrix = glm::mat4(1.0f);
        leftMatrix = glm::translate(leftMatrix, glm::vec3(0.0f, leftPosY, 0.0f));
        program.SetModelMatrix(leftMatrix);

        float left[] = { -4.0f, -1.0f, -3.9f, 0.0f, -4.0f, 0.0f, -3.9f, 0.0f, -4.0f, -1.0f, -3.9f, -1.0f };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, left);
        glEnableVertexAttribArray(program.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(program.positionAttribute);


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Controls for Ball

        if (ballPosY <= -3.6f || ballPosY >= 3.6f)
        {
            ballSpeedY = -ballSpeedY;
        }
        
        if ((ballPosX >= 3.8f))
        {
            if ((ballPosY >= rightPosY + 1.0f) || (ballPosY <= rightPosY - 0.75f))
            {
                ballSpeedX = 0.0f;
                ballSpeedY = 0.0f;
            }
            else
            {
                ballSpeedX = -ballSpeedX;
            }
        }
        else if (ballPosX <= -3.8f)
        {
            if ((ballPosY >= leftPosY + 1.0f) || (ballPosY <= leftPosY - 0.75f))
            {
                ballSpeedX = 0.0f;
                ballSpeedY = 0.0f;
            }
            else
            {
                ballSpeedX = -ballSpeedX;
            }
        }

        ballPosX += ballSpeedX;
        ballPosY += ballSpeedY;

        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(ballPosX, ballPosY, 0.0f));
        program.SetModelMatrix(modelMatrix);

        float ball[] = { -0.1f, -0.1f, 0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, -0.1f, -0.1f, 0.1f, -0.1f };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball);
        glEnableVertexAttribArray(program.positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(program.positionAttribute);

        SDL_GL_SwapWindow(displayWindow);
    }

    Shutdown();
    return 0;
}
