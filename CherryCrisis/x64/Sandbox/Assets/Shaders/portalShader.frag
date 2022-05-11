#version 330 core

out vec4 oColor;

// Varyings
in vec2 vUV;

uniform sampler2D uPortalTexture;

void main()
{
    oColor = texture(uPortalTexture, vUV);
}