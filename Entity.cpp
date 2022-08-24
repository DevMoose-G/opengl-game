#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Entity.hpp>
#include <Util.hpp>

Entity::Entity(){
}

Entity::Entity(const char* objFilepath, int program, glm::mat4 Model){
    MVP = glm::mat4(1.0f);
    Model = Model;
    programID = program;

    loadOBJ(objFilepath, out_vertices, out_uvs, out_normals);

    initialize();
}

Entity::~Entity(){
    glDeleteBuffers(1, &vertexbuffer);
}

void Entity::initialize(){
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, out_vertices.size()*sizeof(glm::vec3), &out_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, out_normals.size()*sizeof(glm::vec3), &out_normals[0], GL_STATIC_DRAW);
}

void Entity::draw(){
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glDrawArrays(GL_TRIANGLES, 0, out_vertices.size()*3);
    printf("Printing Entity with %d vertices\n", out_vertices.size()*3);
}