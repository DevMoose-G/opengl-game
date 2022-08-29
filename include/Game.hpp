#ifndef GAME_HPP
#define GAME_HPP

#include <Inputs.hpp>
#include <Entity.hpp>
#include <Util.hpp>
#include <map>

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
        Entity* controlled = NULL;

        Entity* ground[16];
        int GroundCount = 0;

        // Camera Variables
        glm::vec3 camOffset = glm::vec3(6.0f, 4.0f, 6.0f);
        float horizontalAngle;
        float verticalAngle;
        float MOVE_SPEED = 7.0f;
        float CAM_MOVE_SPEED = 0.05f;

        float gravity = -5.0f;

        // colliders
        GLuint colliderTexture = loadDDS("./resources/texture.dds");
        std::map<Entity*, AABBDisplay> colliders;

        std::map<Entity*, Entity*> followingCreatures;

        // text variables
        unsigned int Text2DTextureID;
        unsigned int Text2DVertexBufferID;
        unsigned int Text2DUVBufferID;
        unsigned int Text2DShaderID;
        unsigned int Text2DUniformID;

        Game();

        void gameLoop(GLFWwindow* window, float deltaTime);
        void toggleProjection();
        void updateProjection();
        void updateMVP(Entity* entity);
        Entity* createEntity(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint textureID);
        Entity* createCreature(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint textureID);
        
        // creation/management of entities
        void setPlayer(Entity* entity);
        void setCreatureOwner(Entity* entity, Entity* creature);
        void addGround(Entity* entity);
        bool isGround(Entity* entity);
        void createColliderDisplays(GLuint programID);

        // game loop functions
        void CheckInputs(GLFWwindow* window, float deltaTime);
        bool checkCollision(Entity* entity1, Entity* entity2);
        void resolveCollision(Entity* entity1, Entity* entity2, float deltaTime);
        // pointer pointing to array of pointers, listing the order of entities
        Entity** drawOrder();

        void initText2D(const char* texturePath);
        void printText2D(const char* text, int x, int y, int size);
        void cleanupText2D();
};

#endif