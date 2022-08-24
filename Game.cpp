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

    // Calculate horizontal & vertical angles
    horizontalAngle += CAM_MOVE_SPEED * float( (float)width/2.0f - xpos) * deltaTime;
    verticalAngle += CAM_MOVE_SPEED * float( (float)height/2.0f - ypos) * deltaTime;

    // vertical limitations
    if(verticalAngle < -0.85) verticalAngle = -0.85;
    if(verticalAngle > 3.14f/2.0f) verticalAngle = 3.14f/2.0f;

    glfwSetCursorPos(window, (float)width/2.0f, (float)height/2.0f);

    glm::vec3 direction = glm::vec3( 
        cos(verticalAngle) * sin(horizontalAngle),  
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );
    glm::vec3 up = glm::cross(right, direction);

    if(glfwGetKey(window, this->input.TOGGLE_ORTHO_PERSPECT) == GLFW_PRESS){
        this->toggleProjection();
    }
    if(glfwGetKey(window, this->input.CAM_RIGHT) == GLFW_PRESS){
        camPosition += right * MOVE_SPEED * deltaTime;
        View = glm::lookAt(camPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }
    if(glfwGetKey(window, this->input.CAM_LEFT) == GLFW_PRESS){
        camPosition -= right * MOVE_SPEED * deltaTime;
        View = glm::lookAt(camPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }
    if(glfwGetKey(window, this->input.CAM_FORWARD) == GLFW_PRESS){
        camPosition += direction * MOVE_SPEED * deltaTime;
        View = glm::lookAt(camPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }
    if(glfwGetKey(window, this->input.CAM_BACKWARD) == GLFW_PRESS){
        camPosition -= direction * MOVE_SPEED * deltaTime;
        View = glm::lookAt(camPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    }

    View = glm::lookAt(
        camPosition, // position 
        camPosition + direction, // direction
        up //up
    );
}

Game::Game(){
    input = Inputs();
    horizontalAngle = 0.0f;
    verticalAngle = 0.0f;

    PerspectiveProjection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
    OrthoProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);

    // starts in Perspective Projection
    projection = PERSPECTIVE;
    View_Projection = PerspectiveProjection * View;

};

void Game::gameLoop(GLFWwindow* window, float deltaTime){
    CheckInputs(window, deltaTime);
    for(int i = 0; i < EntityCount; i++){
        entities[i].draw();
        updateMVP(&(entities[i]));
    }
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

void Game::updateMVP(Entity* entity){
    glUseProgram(entity->programID);
    GLint MVP_Location = glGetUniformLocation(entity->programID, "MVP");
    entity->MVP = View_Projection * entity->Model;
    glUniformMatrix4fv(MVP_Location, 1, 0, (float*)(&entity->MVP));
    updateProjection();
}

Entity* Game::createEntity(const char* objFilepath, int program){
    entities[EntityCount] = Entity(objFilepath, program, glm::mat4(1.0f));
    return &entities[EntityCount++];
}