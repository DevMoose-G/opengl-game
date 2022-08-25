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
        const static int MAX_ENTITIES = 256;
        glm::mat4 View_Projection;
        Inputs input;

        Entity entities[MAX_ENTITIES];
        int EntityCount = 0;
        Entity* player = NULL;

        Entity* ground[16];
        int GroundCount = 0;

        // Camera Variables
        glm::vec3 camPosition = glm::vec3(0.0f, 1.0f, 4.0f);
        float horizontalAngle;
        float verticalAngle;
        float MOVE_SPEED = 15.0f;
        float CAM_MOVE_SPEED = 0.05f;

        float gravity = -5.0f;

        Game();

        void gameLoop(GLFWwindow* window, float deltaTime);
        void toggleProjection();
        void updateProjection();
        void updateMVP(Entity* entity);
        Entity* createEntity(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint textureID);
        void setPlayer(Entity* entity);
        void addGround(Entity* entity);
        bool isGround(Entity* entity);
        // pointer pointing to array of pointers, listing the order of entities
        Entity** drawOrder();

        void CheckInputs(GLFWwindow* window, float deltaTime);
        bool checkCollision(Entity* entity1, Entity* entity2);
};

#endif