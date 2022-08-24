#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <Util.hpp>

class Entity{
    public:
        glm::mat4 MVP = glm::mat4(1.0f);
        glm::mat4 Model = glm::mat4(1.0f);
        int programID = 0;

        std::vector<glm::vec3> out_vertices;
        std::vector<glm::vec2> out_uvs;
        std::vector<glm::vec3> out_normals;

        GLuint vertexbuffer;
        GLuint normalbuffer;

        Entity();
        Entity(const char* objFilepath, int program, glm::mat4 Model);
        ~Entity();

        void initialize();
        void draw();
};

#endif