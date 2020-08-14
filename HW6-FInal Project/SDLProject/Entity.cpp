

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
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && -velocity.x < 0)
    {
        //position.x += penetration_x;
        collidedLeft = true;
    }
    
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0)
    {
        //position.x -= penetration_x;
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
        //position.y -= penetration_y;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0)
    {
        //position.y -= penetration_y;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0)
    {
        //position.y -= penetration_y;
        collidedTop = true;
    }
    
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0)
    {
        //position.y += penetration_y;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0)
    {
        //position.y += penetration_y;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0)
    {
        //position.y += penetration_y;
        collidedBottom = true;
    }
}

void Entity::AIWaitAndGo(Entity* player, Entity* attackObject)
{

    aiState = WALKING;
    
    switch (aiState)
    {
        case IDLE: //All enemies start in IDLE state
            if ((glm::distance(position, player->position) < 10.0)) //Prints true but isn't running unless dead
            {
                switch (aiType)
                {
                    case WALKER:
                        aiState = WALKING;
                        break;
                }
            }
            break;
        case WALKING: //Walking style enemy
            if (player->position.x < position.x)
            {
                velocity.x = -0.25;
            }
            else
            {
                velocity.x = 0.25;
            }
            
            if (player->position.y < position.y)
            {
                velocity.y = -0.25;
            }
            else
            {
                velocity.y = 0.25;
            }
            
            CheckCollisionsX(player, 1);
            CheckCollisionsY(player, 1);
            
            if (collidedLeft || collidedRight || collidedTop || collidedBottom)
            {
                if (player->attackSword)
                {
                    isActive = false;
                    killed = true;
                    player->enemiesKilled++;
                }
                else
                {
                    player->isActive = false;
                    player->killed = true;
                    player->lives--;
                    std::cout << "killed" << std::endl;
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
        default:
            break;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* enemyTarget, Entity* attackObject, Map *map)
{
    if (isActive == false) return;
    
    // Don't really need to redefine all the time as they die when collided upon
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType == ENEMY)
    {
        AI(player, attackObject);
        
        CheckCollisionsY(map);
        CheckCollisionsX(map);
    }
    
    animTime += deltaTime;
    
    if (animTime >= 0.1f)
    {
        animTime = 0.0f;
        animIndex++;
        if (animIndex >= animFrames)
        {
            animIndex = 0;
        }
    }
    
    //movement.x = 0.25;
    //velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(map);
    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);
        
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
