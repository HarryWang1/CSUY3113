//
//  Entity.cpp
//  SDLProject
//
//  Created by Ethan Yao on 3/15/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Entity.hpp"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
    
    speed = 0.0;
    rotateAngle = 0;
    
    animCols = 1;
    animRows = 1;
        
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other) // Checks Collisions
{
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) return true;
    
    return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount) //Checks Y collisions
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            
            if (velocity.y > 0)
            {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0)
            {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount) // Check X Collisions
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        
        if (CheckCollision(object) && isActive && object->isActive)
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            
            if (velocity.x > 0)
            {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                object->collidedRight = true; //Used primarily for changing projectiles
            }
            else if (velocity.x < 0)
            {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                object->collidedLeft = true; //Used primarily for changing projectiles
            }
        }
    }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0)
    {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0)
    {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}

void Entity::CheckCollisionsY(Map *map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0)
    {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0)
    {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0)
    {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0)
    {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0)
    {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0)
    {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::AIWaitAndGo(Entity* player, Entity* attackObject)
{
    switch (aiState)
    {
        case IDLE: //All enemies start in IDLE state
            if (glm::distance(position, player->position) < 6.0f)
            {
                switch (aiType)
                {
                    case JUMPER:
                        aiState = JUMPING;
                        break;
                    case WITCH:
                        aiState = WITCHATTACK;
                        break;
                    case WALKER:
                        aiState = WALKING;
                        break;
                }
            }
            break;
        case WALKING: //Walking style enemy
            if (player->position.x < position.x)
            {
                movement = glm::vec3(-1, 0, 0);
                
                CheckCollisionsX(player, 1);
                
                if (collidedLeft || collidedRight)
                {
                    player->isActive = false;
                    player->killed = true;
                    std::cout << "killed" << std::endl;
                }
            }
            else
            {
                movement = glm::vec3(1, 0, 0);
            }
            break;
        case JUMPING: //Jumping style enemy
            if (player->position.x < position.x)
            {
                movement = glm::vec3(-1, 0, 0);
            }
            else
            {
                movement = glm::vec3(1, 0, 0);
            }
            
            if (collidedBottom)
            {
                velocity.y += 5.0;
                
                collidedBottom = false;
            }
            break;
        case WITCHATTACK: //Witch style enemy
            if (glm::distance(position, player->position) < 3)
            {
                movement = glm::vec3(0);
                
                if (!shootFireball)
                {
                    shootFireball = true;
                    
                    attackObject->isActive = true;
                    attackObject->position = position;
                    attackObject->position.x = position.x - width;
                }
            }
            else
            {
                if (player->position.x < position.x)
                {
                    movement = glm::vec3(-1, 0, 0);
                }
                else
                {
                    movement = glm::vec3(1, 0, 0);
                }
            }

            break;
    }
}

void Entity::AI(Entity* player, Entity* attackObject)
{
    CheckCollisionsX(player, 1);
    CheckCollisionsY(player, 1);
    
    switch (aiType) //All enemies act as a Wait and Go AI
    {
        case WALKER:
            AIWaitAndGo(player);
            break;
        case JUMPER:
            AIWaitAndGo(player);
            break;
        case WITCH:
            AIWaitAndGo(player, attackObject);
            break;
        default:
            break;
    }
}

void Entity::flyingObject(Entity* target, Entity* player, float deltaTime) //Used to control projectiles
{
    collidedRight = false;
    collidedLeft = false;
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(target, 1);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0.0f));
    
    if (collidedRight || collidedLeft)
    {
        isActive = false;
        target->isActive = false;
        
        player->enemiesKilled++;
        player->position.x = -4.0;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* enemyTarget, Entity* attackObject, Map *map)
{
    if (isActive == false) return;
    
    // Don't really need to redefine all the time as they die when collided upon
//    collidedTop = false;
//    collidedBottom = false;
//    collidedLeft = false;
//    collidedRight = false;
    
    if (entityType == ENEMY)
    {
        AI(player, attackObject);
    }
    else if (entityType == ATTACKOBJECT)
    {
        flyingObject(enemyTarget, player, deltaTime);
    }
    
    if (jump)
    {
        jump = false;
        
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(map);
    // CheckCollisionsY(grass, groundCount);
    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);
    // CheckCollisionsX(grass, groundCount);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0.0f));
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animRows) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
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

void Entity::Render(ShaderProgram* program)
{
    if (isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL)
    {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
