#version 330 core

layout (location = 0) in vec3 aPosition;

out vec3 vUVW;

uniform mat4 uViewProjection;

void main()
{
    vUVW = aPosition;
    gl_Position = (uViewProjection * vec4(aPosition, 1.0)).xyww;
}