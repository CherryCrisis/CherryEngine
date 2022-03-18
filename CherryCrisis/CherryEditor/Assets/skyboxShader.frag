#version 330 core

in  vec3 vUVW;

out vec4 FragColor;

uniform samplerCube uTexture;

void main()
{    
    FragColor = texture(uTexture, vUVW);
}