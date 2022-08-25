#version 330 core

in vec2 UV;
in float alpha;

uniform sampler2D textureSampler;

out vec4 color;

void main(){
    color = vec4(texture(textureSampler, UV).rgb, alpha);
}
