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

bool Entity::CheckCollision(Entity* other)
{
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) return true;
    
    return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
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
            }
            else if (velocity.y < 0)
            {
                position.y += penetrationY;
                velocity.y = 0;
            }
        }
    }
}

void Entity::Update(float deltaTime, Entity* rocks, Entity* landing, int rockCount)
{
    if (isActive == false) return;
    
    velocity += acceleration * deltaTime;
    
    position += velocity * deltaTime;
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0.0f));
    
    modelMatrix = glm::rotate(modelMatrix,
                              glm::radians(rotateAngle),
                              glm::vec3(0.0f, 0.0f, 1.0f));
    
    if (rockCount != 0)
    {
        if (CheckCollision(landing))
        {
            isWin = true;
            isActive = false;
            return;
        }
        
        for (int i = 0; i < rockCount; i++)
        {
            if (CheckCollision(&rocks[i]))
            {
                isActive = false;
                return;
            }
        }
    }
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
