#version 330 core

// Varyings
in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPosition;

out vec4 oColor;

struct Material
{
	vec3 albedoColor;
	float	shininess;

};

struct Light
{
	bool isPoint;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NBR_LIGHTS 8
uniform Light uLights[NBR_LIGHTS];

uniform Material uMaterial;

uniform sampler2D uDiffuseTexture;

uniform vec3 uViewPosition;

void getLightColor(out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
	ambient = diffuse = specular = vec3(0.0, 0.0, 0.0);
	
	vec3 normal = normalize(vNormal);
	vec3 viewDir = normalize(uViewPosition - vFragPosition);

	for (int i = 0; i < NBR_LIGHTS; i++)
	{
		Light light = uLights[i];

		// Get light direction, if the light is a point light or a directionnal light
		vec3 lightDir = light.position.xyz - float(light.isPoint) * vFragPosition;
	
		// Compute the light direction and the distance between the fragment and the light
		float distance = length(lightDir);

		// Normalize the light direction manually
		lightDir /= distance;

		// Pre-compute normal ElightDir
		float NdotL = dot(normal, lightDir);

		// Compute ambient
		ambient += uLights[i].ambient;

		// Compute diffuse
		diffuse += max(NdotL, 0.0) * light.diffuse;

		// Compute specular
		#ifdef BLINN_PHONG
			// Blinn phong
			vec3 halfwayDir = normalize(lightDir - viewDir);
			float dotValue = dot(normal, halfwayDir);
		#else
			// Phong
			vec3 reflectDir = lightDir - 2.0 * NdotL * normal; 
			float dotValue = dot(viewDir, reflectDir);
		#endif

		specular += pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess) * light.specular;
	}
}


void main()
{

	vec3 ambient, diffuse, specular;
	
	getLightColor(ambient, diffuse, specular);


	vec3 albedo = texture(uDiffuseTexture, vUV).rgb * diffuse * uMaterial.albedoColor;
    oColor = vec4(albedo, 1.0);
}