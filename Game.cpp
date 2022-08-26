#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>

#include <Game.hpp>
#include <shader.hpp>


void Game::CheckInputs(GLFWwindow* window, float deltaTime){
    // Get mouse position
    int width = 0;
    int height = 0;
    glfwGetWindowSize(window, &width, &height);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if(player == NULL){
        fprintf(stderr, "Player is not set. No Movement is inputted.\n");
        View = glm::lookAt(
            camOffset, // position 
            glm::vec3(0), // direction
            glm::vec3(0, 1, 0) //up
        );
        return;
    } else {
        glm::vec3 motion = glm::vec3(0.0f);
        // Player Movement
        if(glfwGetKey(window, this->input.MOVE_FORWARD) == GLFW_PRESS){
            motion += glm::vec3(0, 0, -MOVE_SPEED * deltaTime);
        }
        if(glfwGetKey(window, this->input.MOVE_BACKWARD) == GLFW_PRESS){
            motion += glm::vec3(0, 0, MOVE_SPEED * deltaTime);
        }
        if(glfwGetKey(window, this->input.MOVE_LEFT) == GLFW_PRESS){
            motion += glm::vec3(-MOVE_SPEED * deltaTime, 0, 0);
        }
        if(glfwGetKey(window, this->input.MOVE_RIGHT) == GLFW_PRESS){
            motion += glm::vec3(MOVE_SPEED * deltaTime, 0, 0);
        }
        if(glm::length(motion) > 0){
            motion = glm::normalize(motion) * MOVE_SPEED * deltaTime;
        }
        player->translate(motion.x, motion.y, motion.z);
    }

    View = glm::lookAt(
        player->position + camOffset, // position 
        player->position, // direction
        glm::vec3(0, 1, 0) //up
    );
}

Game::Game(){
    input = Inputs();
    horizontalAngle = 0.0f;
    verticalAngle = 0.0f;

    PerspectiveProjection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    OrthoProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -14.0f, 100.0f);

    // starts in ORTHO Projection
    projection = ORTHO;
    updateProjection();

};

void Game::setPlayer(Entity* entity){
    player = entity;
}

void Game::setCreatureOwner(Entity* entity, Entity* creature){
    // creature then entity because key has to be unique
    followingCreatures.insert( std::pair<Entity*, Entity*>(creature, entity) );
}

void Game::gameLoop(GLFWwindow* window, float deltaTime){
    CheckInputs(window, deltaTime);

    // undo grounding
    for(int c = 0; c < EntityCount; c++){
        entities[c].isGrounded = false;
    }
    for(int i = 0; i < EntityCount; i++){
        

        // check for collisions
        for(int j = 0; j < EntityCount; j++){
            if( i == j || isGround(&entities[i]) ) continue;

            // else if colliding
            else if(checkCollision(&entities[i], &entities[j])){
                if( isGround(&entities[j]) ){
                    entities[i].isGrounded = true;
                }
            }
        }
    }

    // makes all followingCreatures go to entity that is follows
    for(std::map<Entity*, Entity*>::iterator pair=followingCreatures.begin(); pair != followingCreatures.end(); pair++){
        glm::vec3 distance = pair->second->position - pair->first->position;
        if(pair->first->isGrounded && glm::length(distance) > 1){
            glm::vec3 motion = glm::normalize(distance);
            motion.x *= deltaTime * MOVE_SPEED;
            motion.y *= deltaTime * MOVE_SPEED;
            motion.z *= deltaTime * MOVE_SPEED;
            pair->first->translate(motion.x, motion.y, motion.z);
        }
    }

    Entity** sortedEntities = drawOrder();
    for(int i = 0; i < EntityCount; i++){

        // GRAVITY
        if((!sortedEntities[i]->isGrounded ) && (!isGround(sortedEntities[i]))){
            sortedEntities[i]->translate(0, gravity * deltaTime, 0);
        }

        // update the drawing of the entity
        updateMVP(sortedEntities[i]);
        sortedEntities[i]->draw();
    }
    // frees the memoryAllocated for sorting the Entities
    free(sortedEntities);

    // now draw all the colliderDisplays if any
    for(std::map<Entity*, AABBDisplay>::iterator pair=colliders.begin(); pair != colliders.end(); pair++){
        pair->second.gameLoop();
        updateMVP(&pair->second);
        pair->second.draw();
    }
}

void Game::addGround(Entity* entity){
    ground[GroundCount++] = entity;
}

bool Game::isGround(Entity* entity){
    bool present = false;
    for(int i = 0; i < GroundCount; i++){
        if(entity == ground[i]){
            present = true;
        }
    }
    return present;
}

void Game::toggleProjection(){
    if(projection == PERSPECTIVE){
        projection = ORTHO;
    } else if(projection == ORTHO){
        projection = PERSPECTIVE;
    }
    updateProjection();
}

void Game::updateProjection(){
    if(projection == PERSPECTIVE){
        View_Projection = PerspectiveProjection * View;
    } else if(projection == ORTHO){
        View_Projection = OrthoProjection * View;
    }
}

// runs every game loop for every entity
void Game::updateMVP(Entity* entity){
    entity->update(); // updates entity position
    glUseProgram(entity->programID);

    // update all uniforms
    GLint MVP_Location = glGetUniformLocation(entity->programID, "MVP");
    GLint Transparency_Location = glGetUniformLocation(entity->programID, "transparency");
    entity->MVP = View_Projection * entity->Model;
    glUniformMatrix4fv(MVP_Location, 1, 0, (float*)(&entity->MVP));
    glUniform1f(Transparency_Location, entity->transparency);

    updateProjection();
}

// generate all the AABBDisplays needed
void Game::createColliderDisplays(GLuint programID){
    for(int i = 0; i < EntityCount; i++){
        AABBDisplay colliderDisplay = AABBDisplay(&entities[i], programID, colliderTexture);
        colliders.insert( std::pair<Entity*, AABBDisplay>(&entities[i], colliderDisplay) );
    }
}

// returns entities based on transparency if solid, display first
Entity** Game::drawOrder(){
    Entity** sortedEntities = (Entity**)malloc(sizeof(Entity*) * EntityCount);
    int index = 0;
    // have solid entities first
    for(int i = 0; i < EntityCount; i++){
        if(entities[i].transparency == 1){
            sortedEntities[index++] = &entities[i];
        }
    }
    // then transparent/translucent afterwards
    for(int i = 0; i < EntityCount; i++){
        if(entities[i].transparency != 1){
            sortedEntities[index++] = &entities[i];
        }
    }

    return sortedEntities;
}

Entity* Game::createEntity(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint textureID){
    entities[EntityCount] = Entity(name, objFilepath, position, program, textureID);
    return &entities[EntityCount++];
}

// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
bool Game::checkCollision(Entity* entity1, Entity* entity2){
    AABB collider1 = getGlobalAABB(entity1);
    AABB collider2 = getGlobalAABB(entity2);
    return (collider1.minX <= collider2.maxX && collider1.maxX >= collider2.minX)
        && (collider1.minY <= collider2.maxY && collider1.maxY >= collider2.minY)
        && (collider1.minZ <= collider2.maxZ && collider1.maxZ >= collider2.minZ);
}