#version 330 core

// Varyings
in vec2 vUV;
in vec3 vNormal;

out vec4 oColor;

struct Material
{
	vec4 albedoColor;
};

uniform Material uMaterial;

uniform sampler2D uDiffuseTexture;

void main()
{
	vec4 albedo = texture(uDiffuseTexture, vUV) * uMaterial.albedoColor;
    oColor = albedo;
}