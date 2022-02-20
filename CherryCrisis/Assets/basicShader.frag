#version 330 core

// Varyings
in vec2 vUV;
in vec3 vNormal;

out vec4 oColor;

struct Material
{
	vec3 albedoColor;
};

uniform Material uMaterial;

uniform sampler2D uDiffuseTexture;

void main()
{
	vec3 albedo = texture(uDiffuseTexture, vUV).rgb * uMaterial.albedoColor;
    oColor = vec4(albedo, 1.0);
}