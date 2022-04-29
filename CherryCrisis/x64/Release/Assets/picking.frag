#version 330 core

// Varyings
out vec4 color;
uniform vec4 uColorID;


void main()
{
    color = uColorID;
}