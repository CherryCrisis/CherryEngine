#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uViewProjection;

out vec2 vUV;
out vec3 vNormal;

void main()
{
    vUV = aUV;
    vNormal = aNormal;

    vec4 pos4 = uModel * vec4(aPosition, 1.0);
    gl_Position = uViewProjection * pos4;
}