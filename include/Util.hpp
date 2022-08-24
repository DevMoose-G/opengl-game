#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>

GLuint loadDDS(const char* imagepath);
bool loadOBJ( const char* path,  std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals );

#endif