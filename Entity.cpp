#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Entity.hpp>
#include <Util.hpp>

AABB getGlobalAABB(Entity *entity){
    AABB global_collider = getScaledAABB(entity);

    global_collider.minX += entity->position.x;
    global_collider.minY += entity->position.y;
    global_collider.minZ += entity->position.z;
    global_collider.maxX += entity->position.x;
    global_collider.maxY += entity->position.y;
    global_collider.maxZ += entity->position.z;
    
    return global_collider;
}

AABB getScaledAABB(Entity *entity){
    AABB scale_collider = entity->collider.copy();
    scale_collider.minX *= entity->_scale.x;
    scale_collider.minY *= entity->_scale.y;
    scale_collider.minZ *= entity->_scale.z;
    scale_collider.maxX *= entity->_scale.x;
    scale_collider.maxY *= entity->_scale.y;
    scale_collider.maxZ *= entity->_scale.z;
    return scale_collider;
}

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

    if(objFilepath != ""){
        loadOBJ(objFilepath, out_vertices, out_uvs, out_normals);
    }
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

void Entity::translate(float x, float y, float z){
    position.x += x;
    position.y += y;
    position.z += z;
    // no longer know if it is grounded
    isGrounded = false;
}

void Entity::translate(glm::vec3 motion){
    translate(motion.x, motion.y, motion.z);
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
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotatedTranslate = glm::rotate(translateMatrix, rotation, glm::vec3(0, 1, 0));
    Model = glm::scale(rotatedTranslate, _scale);
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

AABBDisplay::AABBDisplay(Entity* entity, int programID, GLuint colliderTexture): Entity("AABBDisplay", "./resources/cube.obj", entity->position, programID, colliderTexture){
    this->entity = entity;
    AABB globalAABB = getGlobalAABB(entity);
    this->scale(entity->collider.maxX-entity->collider.minX, entity->collider.maxY-entity->collider.minY, entity->collider.maxZ-entity->collider.minZ);
    this->_scale *= entity->_scale;
    this->_scale /= 2.0f;
    this->_scale *= 1.01f; // bit bigger
    this->transparency = 0.5f;
}

void AABBDisplay::gameLoop(){
    // update vertices
    this->scale(entity->collider.maxX-entity->collider.minX, entity->collider.maxY-entity->collider.minY, entity->collider.maxZ-entity->collider.minZ);
    this->_scale *= entity->_scale;
    this->_scale /= 2.0f;
    this->_scale *= 1.01f; // bit bigger

    // update position
    position = entity->position;
}