#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 normal;

// Model, View, Projection matrix
uniform mat4 MVP;
uniform float transparency = 1.0;

out vec2 UV;
out float alpha;

void main(){
    gl_Position = MVP * vec4(position, 1);
    alpha = transparency;
    UV = vertexUV;
}