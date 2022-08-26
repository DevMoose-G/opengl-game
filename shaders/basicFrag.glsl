#version 330 core

in float alpha;

out vec4 color;

void main(){
    color = vec4(1, 0, 1, alpha);
}
