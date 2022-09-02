#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <cmath>

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
        if(glfwGetKey(window, this->input.TOGGLE_CREATURE) == GLFW_PRESS){
            if(input.TOGGLE_CREATURE_PRESSED) {}
            else{
				// toggles between different "players" (player & creatures)
				switchPlayer();
            }
        } else  input.TOGGLE_CREATURE_PRESSED = false; // detects when you release toggle

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
        controlled->motion += glm::vec3(motion.x, motion.y, motion.z);
    }

    View = glm::lookAt(
        controlled->position + camOffset, // position 
        controlled->position, // direction
        glm::vec3(0, 1, 0) //up
    );
}

void Game::switchPlayer(){
	// lets you cycle through your creatures
	input.TOGGLE_CREATURE_PRESSED = true;
	std::vector<Entity*> creatureOrder;
	
	if(!battleMode){
		// player can't move in battle mode
		creatureOrder.push_back(player);
	}

	std::vector<Creature*> creaturesArray = ((Trainer*)player)->creatures;
	for(int i = 0; i < creaturesArray.size(); i++){
		creatureOrder.push_back(creaturesArray[i]);
	}

	bool switched = false;
	for(int i = 0; i < creatureOrder.size(); i++){
		if(creatureOrder[i] == controlled){
			if(i == creatureOrder.size() - 1) controlled = creatureOrder[0];
			else    controlled = creatureOrder[i+1];
			switched = true;
			break;
		}
	}
	// in case controlled no longer exists in creatureOrder, you control the first creature
	if(! switched){
		controlled = creatureOrder[0];
	}
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
    if(player == NULL){
        player = entity;
    }
    controlled = entity;
}

void Game::setCreatureOwner(Trainer* entity, Creature* creature){
    // creature then entity because key has to be unique
	entity->creatures.push_back(creature);
}

void Game::gameLoop(GLFWwindow* window, float deltaTime){

	// resets all motion
    for(int i = 0; i < EntityCount; i++){
        entities[i]->motion = glm::vec3(0, 0, 0);
    }

    CheckInputs(window, deltaTime);

	// does all the game loops in each creature
	for(int i = 0; i < EntityCount; i++){
		if(strcmp(entities[i]->type.c_str(), "Creature") == 0){
			Creature* creature = (Creature*) entities[i];
			creature->gameLoop();
		}
	}

    /* first do motion */
    
	if(battleMode){
		if(glfwGetKey(window, input.CREATURE_ATTACK) == GLFW_PRESS){
			Creature* creature = (Creature*) controlled;
			// creates a telegraph if not one already
			if(strcmp(creature->currentTelegraph.type.c_str(), "Telegraph") != 0){
				GLuint shaders = LoadShaders("./shaders/basicVertex.glsl", "./shaders/basicFrag.glsl");
				creature->currentTelegraph = Telegraph("./resources/charge_telegraph.obj", creature->position, shaders);

				addEntity(&creature->currentTelegraph);
			}
		} else if(glfwGetKey(window, input.CREATURE_ATTACK) == GLFW_RELEASE){
			Creature* creature = (Creature*) controlled;
			// deletes a telegraph if there is one
			if(strcmp(creature->currentTelegraph.type.c_str(), "Telegraph") == 0){
				removeEntity(&creature->currentTelegraph);
				creature->currentTelegraph = Telegraph();
			}
		}
	}
	else {
		// makes all followingCreatures go to entity that is follows
		for(int i = 0; i < EntityCount; i++){
			if(strcmp(entities[i]->type.c_str(), "Trainer") == 0){
				Trainer* trainer = (Trainer*)entities[i];
				for(int j = 0; j < trainer->creatures.size(); j++){
					Creature* creature = trainer->creatures[j];
					glm::vec3 distance = trainer->position - creature->position;
					// teleport creature to player if too far away
					if(glm::length(distance) > 25.0f  && creature != controlled){
						glm::vec3 playerPos = trainer->position;
						creature->position = playerPos + glm::vec3(0, 1, 0);
					}
					// else just move them closer
					if(creature->isGrounded && glm::length(distance) > 1.35f && creature != controlled){
						glm::vec3 motion = glm::normalize(distance);
						creature->motion.x = motion.x * deltaTime * MOVE_SPEED;
						creature->motion.y = motion.y * deltaTime * MOVE_SPEED;
						creature->motion.z = motion.z * deltaTime * MOVE_SPEED;
					}
				}
			}
		}
	}

    for(int i = 0; i < EntityCount; i++){
        // GRAVITY (only if not grounded and you are not ground and you are colliding)
        if((!entities[i]->isGrounded ) && (!isGround(entities[i])) && entities[i]->collisions){
            entities[i]->motion += glm::vec3(0, gravity * deltaTime, 0);
        }
    }

    // actual movement & rotation after calculations
    for(int i = 0; i < EntityCount; i++){
        entities[i]->translate(entities[i]->motion.x, entities[i]->motion.y, entities[i]->motion.z);

        // only rotation in the x and z plane, so not vertical rotation
        glm::vec2 flatMotion = glm::vec2(entities[i]->motion.x, entities[i]->motion.z);
        if(glm::length(flatMotion) > 0.05f){
            flatMotion = glm::normalize(flatMotion);
            entities[i]->rotation = std::atan2(flatMotion.x, flatMotion.y);
        }
    }

    /* then detect collisions & resolve them */
    for(int i = 0; i < EntityCount; i++){
        // check for collisions
        for(int j = 0; j < EntityCount; j++){
            if( i == j || isGround(entities[i]) ) continue;

            // else if colliding
            else if(checkCollision(entities[i], entities[j])){
                resolveCollision(entities[i], entities[j], deltaTime);
                if( isGround(entities[j]) ){
                    entities[i]->isGrounded = true;
                }

				// check if they are two trainers colliding
				if( strcmp(entities[i]->type.c_str(), "Trainer") == 0 
					&& strcmp(entities[j]->type.c_str(), "Trainer") == 0){
					
					battleMode = true;
					if(entities[i] == player){
						battleTrainer1 = (Trainer*)entities[i];
						battleTrainer2 = (Trainer*)entities[j];
					} else if(entities[j] == player){
						battleTrainer1 = (Trainer*)entities[j];
						battleTrainer2 = (Trainer*)entities[i];
					}
					else	fprintf(stderr, "Haven't coded how to deal with non-player battles.\n");
					
					switchPlayer();
					glm::vec3 centerPos = entities[i]->position + entities[j]->position;
					centerPos /= 2; // midpoint b/w trainers
					entities[i]->position = centerPos + glm::vec3(ArenaSize.x/2, 0, ArenaSize.y/2);
					entities[j]->position = centerPos - glm::vec3(ArenaSize.x/2, 0, ArenaSize.y/2);
					printf("Battle mode engaged.\n");
				}
            }
        }
    }

    /* draw */
    Entity** sortedEntities = drawOrder();
    for(int i = 0; i < EntityCount; i++){
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
        AABBDisplay colliderDisplay = AABBDisplay(entities[i], programID, colliderTexture);
        colliders.insert( std::pair<Entity*, AABBDisplay>(entities[i], colliderDisplay) );
    }
}

// returns entities based on transparency if solid, display first
Entity** Game::drawOrder(){
    Entity** sortedEntities = (Entity**)malloc(sizeof(Entity*) * EntityCount);
    int index = 0;
    // have solid entities first
    for(int i = 0; i < EntityCount; i++){
        if(entities[i]->transparency == 1){
            sortedEntities[index++] = entities[i];
        }
    }
    // then transparent/translucent afterwards
    for(int i = 0; i < EntityCount; i++){
        if(entities[i]->transparency != 1){
            sortedEntities[index++] = entities[i];
        }
    }

    return sortedEntities;
}

void Game::removeEntity(Entity* entity){
	glDeleteBuffers(1, &entity->vertexbuffer);
	glDeleteBuffers(1, &entity->normalbuffer);
	for(int i = 0; i < EntityCount; i++){
		if(entities[i] == entity){
			// move the last entity to this spot and reduce EntityCount
			entities[i] = entities[EntityCount - 1];
			entities[EntityCount - 1] = nullptr;
			EntityCount--;
		}
	}
}

void Game::addEntity(Entity* entity){
	// creates the vertex buffers for the entity
	glGenBuffers(1, &entity->vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, entity->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, entity->out_vertices.size()*sizeof(glm::vec3), &entity->out_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &entity->normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, entity->normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, entity->out_normals.size()*sizeof(glm::vec3), &entity->out_normals[0], GL_STATIC_DRAW);

	// the collision displays
	// glGenBuffers(1, &pair->second.vertexbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, pair->second.vertexbuffer);
	// glBufferData(GL_ARRAY_BUFFER, pair->second.out_vertices.size()*sizeof(glm::vec3), &pair->second.out_vertices[0], GL_STATIC_DRAW);

	// glGenBuffers(1, &pair->second.normalbuffer);
	// glBindBuffer(GL_ARRAY_BUFFER, pair->second.normalbuffer);
	// glBufferData(GL_ARRAY_BUFFER, pair->second.out_normals.size()*sizeof(glm::vec3), &pair->second.out_normals[0], GL_STATIC_DRAW);

    entities[EntityCount++] = entity;
}

// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
bool Game::checkCollision(Entity* entity1, Entity* entity2){
	if(!entity1->collisions || !entity2->collisions){
		return false;
	}
    AABB collider1 = getGlobalAABB(entity1);
    AABB collider2 = getGlobalAABB(entity2);
    return (collider1.minX <= collider2.maxX && collider1.maxX >= collider2.minX)
        && (collider1.minY <= collider2.maxY && collider1.maxY >= collider2.minY)
        && (collider1.minZ <= collider2.maxZ && collider1.maxZ >= collider2.minZ);
}


void Game::resolveCollision(Entity* entity1, Entity* entity2, float deltaTime){

    if(! checkCollision(entity1, entity2) )  return;

    AABB collider1 = getScaledAABB(entity1);
    AABB collider2 = getScaledAABB(entity2);

    glm::vec3 distance = entity2->position - entity1->position;

    Entity* ground = NULL;
    // no collision b/w two grounds
    if(isGround(entity1) && isGround(entity2))  return;
    if(isGround(entity1))   ground = entity1;
    if(isGround(entity2))   ground = entity2;

    // figure out which side overlaps left or right
    // (entity1.maxX-entity2.minX) or (entity2.maxX-entity1.minX)
    float overlapX1 = (collider1.maxX - collider2.minX);
    float overlapX2 = (collider2.maxX - collider1.minX);
    // entity1 is left & entity2 is right
    bool overlapXLeftRight = false;
    if(overlapX1 > overlapX2){
        overlapXLeftRight = true;
    }

    float overlapY1 = (collider1.maxY - collider2.minY);
    float overlapY2 = (collider2.maxY - collider1.minY);
    bool overlapYLeftRight = false;
    if(overlapY1 > overlapY2){
        overlapYLeftRight = true;
    }

    float overlapZ1 = (collider1.maxZ - collider2.minZ);
    float overlapZ2 = (collider2.maxZ - collider1.minZ);
    bool overlapZLeftRight = false;
    if(overlapZ1 > overlapZ2){
        overlapZLeftRight = true;
    }

    // the overlap is the value after these subtractions
    if(overlapXLeftRight){
        distance.x -= std::abs(collider1.maxX);
        distance.x -= std::abs(collider2.minX);
    } else {
        distance.x -= std::abs(collider2.maxX);
        distance.x -= std::abs(collider1.minX);
    }
    if(overlapYLeftRight){
        distance.y -= std::abs(collider1.maxY);
        distance.y -= std::abs(collider2.minY);
    } else {
        distance.y -= std::abs(collider2.maxY);
        distance.y -= std::abs(collider1.minY);
    }
    if(overlapZLeftRight){
        distance.z -= std::abs(collider1.maxZ);
        distance.z -= std::abs(collider2.minZ);
    } else {
        distance.z -= std::abs(collider2.maxZ);
        distance.z -= std::abs(collider1.minZ);
    }
    // essentially the absolute value of distance after aabb subtraction which is the amount of overlap in each axis
    glm::vec3 overlap = glm::vec3(-distance.x, -distance.y, -distance.z);

    // if ground collision, only move entity1
    if(ground != NULL){
        Entity* moveable = (ground != entity1) ? entity1 : entity2;

        // finds the minimum path to become uncollided
        if(overlap.x < overlap.y && overlap.x < overlap.z){
            // flip the motion in the x axis
            moveable->translate(-moveable->motion.x, 0, 0);
        } else if(overlap.y < overlap.x && overlap.y < overlap.z){
            // flip the motion in the y axis
            moveable->translate(0, -moveable->motion.y, 0);
        } else{
            // flip the motion in the z axis
            moveable->translate(0, 0, -moveable->motion.z);
        }

    } else {

        float weightRatio = entity1->weight / entity2->weight;
        glm::vec3 motion1 = (glm::length(entity1->motion) > glm::length(entity2->motion)) ? entity1->motion : -entity2->motion;
        glm::vec3 motion2 = (glm::length(entity1->motion) > glm::length(entity2->motion)) ? -entity1->motion : entity2->motion;

        if(overlap.x < overlap.y && overlap.x < overlap.z){
            // flip the motion in the x axis
            entity1->translate(-motion1.x / weightRatio, 0, 0);
            entity2->translate(-motion2.x * weightRatio, 0, 0);
        } else if(overlap.y < overlap.x && overlap.y < overlap.z){
            // flip the motion in the y axis
            entity1->translate(0, -motion1.y / weightRatio, 0);
            entity2->translate(0, -motion2.y * weightRatio, 0);
        } else {
            // flip the motion in the z axis
            entity1->translate(0, 0, -motion1.z / weightRatio);
            entity2->translate(0, 0, -motion2.z * weightRatio);
        }
    }
}


void Game::initText2D(const char* texturePath){
    Text2DTextureID = loadDDS(texturePath);

    glGenBuffers(1, &Text2DVertexBufferID);
    glGenBuffers(1, &Text2DUVBufferID);

    Text2DShaderID = LoadShaders("./shaders/text2DVertex.glsl", "./shaders/text2DFrag.glsl");

    Text2DUniformID = glGetUniformLocation( Text2DShaderID, "myTextureSampler" );
}

void Game::printText2D(const char* text, int x, int y, int size){
    unsigned int length = strlen(text);
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> UVs;

    for(unsigned int i = 0; i < length; i++){
        glm::vec2 vertex_up_left    = glm::vec2( x+i*size     , y+size );
		glm::vec2 vertex_up_right   = glm::vec2( x+i*size+size, y+size );
		glm::vec2 vertex_down_right = glm::vec2( x+i*size+size, y      );
		glm::vec2 vertex_down_left  = glm::vec2( x+i*size     , y      );

		vertices.push_back(vertex_up_left   );
		vertices.push_back(vertex_down_left );
		vertices.push_back(vertex_up_right  );

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character%16)/16.0f;
		float uv_y = (character/16)/16.0f;

		glm::vec2 uv_up_left    = glm::vec2( uv_x           , uv_y );
		glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/16.0f, uv_y );
		glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) );
		glm::vec2 uv_down_left  = glm::vec2( uv_x           , (uv_y + 1.0f/16.0f) );
		UVs.push_back(uv_up_left   );
		UVs.push_back(uv_down_left );
		UVs.push_back(uv_up_right  );

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
    }

    glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
    glBufferData(GL_ARRAY_BUFFER, UVs.size()*sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

    glUseProgram(Text2DShaderID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
    glUniform1i(Text2DUniformID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void Game::cleanupText2D(){
    glDeleteBuffers(1, &Text2DVertexBufferID);
    glDeleteBuffers(1, &Text2DUVBufferID);

    glDeleteTextures(1, &Text2DTextureID);

    glDeleteProgram(Text2DShaderID);
}
