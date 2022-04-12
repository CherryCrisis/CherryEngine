#version 330 core

//#define BLINN_PHONG

// Varyings
in VS_OUT
{
	vec2 vUV;
	vec3 vNormal;
	vec3 vFragPosition;
} fs_in;

out vec4 oColor;

struct Material
{
	vec3	ambientCol;
	vec3	diffuseCol;
	vec3	specularCol;
	vec3	emissiveCol;
	float	shininess;

	sampler2D albedoTex;
	sampler2D normalMap;
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

uniform vec3 uViewPosition;

void getLightColor(in vec3 normal, out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
	ambient = diffuse = specular = vec3(0.0);
	
	vec3 viewDir = normalize(uViewPosition - fs_in.vFragPosition);

	for (int i = 0; i < NBR_LIGHTS; i++)
	{
		Light light = uLights[i];

		// Compute ambient
		ambient += light.ambient;

		// Get light direction, if the light is a point light or a directionnal light
		vec3 lightDir = light.position - float(light.isPoint) * fs_in.vFragPosition;
	
		// Compute the light direction and the distance between the fragment and the light
		// Normalize it manually
		float lightDist = length(lightDir);
		lightDir /= lightDist;

		// Pre-compute normal lightDir
		float NdotL = dot(normal, lightDir);

		float diffuseVal = max(NdotL, 0.0);

		if (diffuseVal <= 0)
			continue;

		// Compute diffuse
		diffuse += diffuseVal * light.diffuse;

		// Compute specular
		#ifdef BLINN_PHONG
			// Blinn phong
			vec3 H = normalize(lightDir + viewDir);
			float dotValue = dot(normal, H);
		#else
			// Phong
			vec3 reflectDir = 2.0 * NdotL * normal - lightDir; 
			float dotValue = dot(viewDir, reflectDir);
		#endif

		specular += pow(clamp(dotValue, 0.0, 1.0), uMaterial.shininess) * light.specular;
	}
}

vec3 getShadedColor(in vec3 normal)
{
	vec3 ambient, diffuse, specular;

	getLightColor(normal, ambient, diffuse, specular);

	vec3 ambientColor = uMaterial.ambientCol * ambient;

	vec3 diffuseColor = uMaterial.diffuseCol * diffuse;

	vec3 albedoColor = (ambientColor + diffuseColor) * texture(uMaterial.albedoTex, fs_in.vUV).rgb;

	vec3 specularColor = uMaterial.specularCol * specular;

	vec3 emissiveColor = uMaterial.emissiveCol;

	return albedoColor + specularColor + emissiveColor;
}

void main()
{
	//vec3 N = normalize(texture(uMaterial.normalMap, fs_in.vUV).xyz * 2.0 - 1.0);


	vec3 shadedColor = getShadedColor(fs_in.vNormal);
    oColor = vec4(shadedColor, 1.0);
}