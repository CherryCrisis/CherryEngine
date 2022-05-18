#version 330 core

out vec4 oColor;

// Varyings
in vec4 uScreenPos;

uniform sampler2D uPortalTexture;

void main()
{
	vec2 screenSpaceUV = uScreenPos.xy / uScreenPos.w;
    oColor = texture(uPortalTexture, screenSpaceUV);
}