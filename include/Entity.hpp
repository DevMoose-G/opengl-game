#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <Util.hpp>

// Axis-Aligned Bounding Box
class AABB{
    public:
        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;

        AABB();
        AABB(float lowX, float lowY, float lowZ, float highX, float highY, float highZ);

        AABB copy();
};

class Entity{
    public:
        glm::mat4 MVP = glm::mat4(1.0f);
        glm::mat4 Model = glm::mat4(1.0f);
        int programID = 0;

        std::vector<glm::vec3> out_vertices;
        std::vector<glm::vec2> out_uvs;
        std::vector<glm::vec3> out_normals;

        GLuint vertexbuffer;
        GLuint uvbuffer;
        GLuint normalbuffer;

        GLuint textureID;

        std::string _name;

        glm::vec3 position;
        glm::vec3 _scale;

        glm::vec3 motion;

        float transparency = 1.0f;

        bool isGrounded;

        AABB collider;

        Entity();
        Entity(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint texture);
        ~Entity();

        void AABBFromVertices();
        void initialize();
        void translate(float x, float y, float z);
        void translate(glm::vec3 motion);
        void scale(float s);
        void scale(float x, float y, float z);
        void setTransparency(float alpha);
        void update();
        void draw();
};

// shows the actual collision box
class AABBDisplay: public Entity {
    public:
        Entity* entity;

        AABBDisplay(Entity* entity, int programID, GLuint colliderTexture);

        void gameLoop();
};

AABB getGlobalAABB(Entity* entity);
AABB getScaledAABB(Entity *entity);

#endif