#version 430 core

out vec4 oColor;

uniform vec4 uColor;

void main()
{
    oColor = uColor;
}