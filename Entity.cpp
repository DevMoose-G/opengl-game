#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Entity.hpp>
#include <Util.hpp>

AABB::AABB(float lowX, float lowY, float lowZ, float highX, float highY, float highZ){
    minX = lowX;
    minY = lowY;
    minZ = lowZ;
    maxX = highX;
    maxY = highY;
    maxZ = highZ;
}

AABB::AABB(){

}

AABB AABB::copy(){
    return AABB(minX, minY, minZ, maxX, maxY, maxZ);
}

Entity::Entity(){
}

Entity::Entity(const char* name, const char* objFilepath, glm::vec3 position, int program, GLuint texture){
    this->_name = name;
    textureID = texture;
    Model = glm::translate(glm::mat4(1.0f), position);
    _scale = glm::vec3(1.0f);
    this->position = position;
    programID = program;

    loadOBJ(objFilepath, out_vertices, out_uvs, out_normals);
    AABBFromVertices();
    initialize();
}

Entity::~Entity(){
    glDeleteBuffers(1, &vertexbuffer);
}

// creates an AABB from the farthest vertices
void Entity::AABBFromVertices(){
    glm::vec3 vertex = out_vertices[0];
    collider = AABB(vertex.x, vertex.y, vertex.z, vertex.x, vertex.y, vertex.z);
    for(int i = 1; i < out_vertices.size(); i++){
        vertex = out_vertices[i];
        collider.minX = (vertex.x < collider.minX) ? vertex.x : collider.minX;
        collider.minY = (vertex.y < collider.minY) ? vertex.y : collider.minY;
        collider.minZ = (vertex.z < collider.minZ) ? vertex.z : collider.minZ;
        collider.maxX = (vertex.x > collider.maxX) ? vertex.x : collider.maxX;
        collider.maxY = (vertex.y > collider.maxY) ? vertex.y : collider.maxY;
        collider.maxZ = (vertex.z > collider.maxZ) ? vertex.z : collider.maxZ;
    }
}

void Entity::initialize(){
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, out_vertices.size()*sizeof(glm::vec3), &out_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, out_uvs.size()*sizeof(glm::vec2), &out_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, out_normals.size()*sizeof(glm::vec3), &out_normals[0], GL_STATIC_DRAW);
}

AABB Entity::getGlobalAABB(){
    AABB global_collider = collider.copy();
    global_collider.minX += position.x;
    global_collider.minY += position.y;
    global_collider.minZ += position.z;
    global_collider.maxX += position.x;
    global_collider.maxY += position.y;
    global_collider.maxZ += position.z;
    return global_collider;
}

void Entity::translate(float x, float y, float z){
    position.x += x;
    position.y += y;
    position.z += z;
}

void Entity::scale(float s){
    scale(s, s, s);
}

void Entity::scale(float x, float y, float z){
    _scale = glm::vec3(x, y, z);
}

void Entity::setTransparency(float alpha){
    transparency = alpha;
}

void Entity::update(){
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), _scale);
    Model = glm::translate(scaleMatrix, position);
}

void Entity::draw(){
    glBindTexture(GL_TEXTURE_2D, textureID);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    glDrawArrays(GL_TRIANGLES, 0, out_vertices.size()*sizeof(glm::vec3));
}