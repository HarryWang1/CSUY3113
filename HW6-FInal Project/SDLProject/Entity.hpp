

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>

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
#include "Map.hpp"

enum EntityType { PLAYER, PLATFORM, ENEMY, ATTACKOBJECT };

enum AIType { WALKER };
enum AIState { IDLE, WALKING };

class Entity
{
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float speed;
    float jumpPower = 0.0;
    
    float rotateAngle;
    
    std::string direction = "Right";
    
    int *animIndices = NULL;
    int animFrames = 4;
    int animIndex = 0;
    int animTime = 0.0f;
    int animCols = 4;
    int animRows = 4;
    
    int lives = 1;
    
    int enemiesKilled = 0;
    
    float width = 1.0;
    float height = 1.0;
    
    bool isActive = true;
    bool isWin = false;
    
    bool killed = false;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    bool attackSword = false;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    Entity();
    
    void Update(float deltaTime, Entity* player, Entity* enemyTarget, Entity* attackObject, Map *map);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);
    
    void AI(Entity* player, Entity* attackObject);
    void AIWaitAndGo(Entity* player, Entity* attackObject = nullptr);        
};

#endif /* Entity_hpp */
