#ifndef GAME_HPP
#define GAME_HPP

#include <Inputs.hpp>
#include <Entity.hpp>
#include <Util.hpp>

enum Projection { PERSPECTIVE, ORTHO };

class Game{
    private:
        glm::mat4 PerspectiveProjection;
        glm::mat4 OrthoProjection;
        glm::mat4 View;

        Projection projection;

    public:
        glm::mat4 View_Projection;
        Inputs input;
        Entity entities[1024];
        int EntityCount = 0;

        // Camera Variables
        glm::vec3 camPosition = glm::vec3(0.0f, 1.0f, 4.0f);
        float horizontalAngle;
        float verticalAngle;

        float MOVE_SPEED = 5.0f;
        float CAM_MOVE_SPEED = 0.05f;

        Game();

        void gameLoop(GLFWwindow* window, float deltaTime);
        void toggleProjection();
        void updateProjection();
        void updateMVP(Entity* entity);
        Entity* createEntity(const char* objFilepath, int program);

        void CheckInputs(GLFWwindow* window, float deltaTime);
};

#endif