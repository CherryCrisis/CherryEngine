#version 330 core

in  vec3 vUVW;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec4 oBrightColor;

uniform samplerCube uTexture;

void main()
{    
    oColor = texture(uTexture, vUVW);
    oBrightColor = vec4(0.0);
}