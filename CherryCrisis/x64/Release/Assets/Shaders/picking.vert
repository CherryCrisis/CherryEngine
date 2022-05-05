#version 330 core

// Attributes
layout(location = 0) in vec3 aPosition;

// Uniforms
uniform mat4 uViewProjection;
uniform mat4 uModel;

void main()
{

    vec4 pos4 = (uModel * vec4(aPosition, 1.0));
    gl_Position = uViewProjection * pos4;
}