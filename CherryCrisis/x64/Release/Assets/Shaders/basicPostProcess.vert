#version 330 core

// Attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

// Varyings (variables that are passed to fragment shader with perspective interpolation)
out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = vec4(aPosition, 1.0);
}